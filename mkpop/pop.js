#!/usr/local/bin/nodejs

const fs = require("fs");
const Canvas = require('canvas');
const Image = Canvas.Image;

const palette = // fs.readFileSync('paletteFinal.hex', 'utf-8')
`181c20
4a5052
a4a19c
ffffff
202c9c
5255ff
08a18b
39b2de
8b20ac
f691a4
a42010
ff8518
ffde39
734429
527d10
83ce18`
      .split('\n')
      .filter( x => !!x.length )
      .map( x => x.split(/([a-f0-9][a-f0-9])/).filter( x=>!!x.length ).map(x=>parseInt(x, 16)) );
/*
fs.writeFileSync(
    'palette.hex',
    palette.map( c => c[0].toString(16).padStart(2,'0') +
		 c[1].toString(16).padStart(2,'0') + 
		 c[2].toString(16).padStart(2,'0')
	       ).join('\n'),
    'utf-8'
);
*/

const tmpab32 = new Int32Array(1);
const tmpab8 = new Int8Array( tmpab32.buffer );

function STR(s){
    return [
	...U32( s.length+1 ),
	s.split("").map( s=>s.charCodeAt(0) ),
	0
    ];
}

function U32(n){
    tmpab32[0] = n;
    return tmpab8;
}

function img4bpp( tagType, path, w, h ){
    const canvas = new Canvas(w, h);
    const ctx = canvas.getContext('2d');
    const image = new Image();
    image.src = fs.readFileSync( path );
    
    ctx.drawImage( image, 0, 0, w, h );
    const buf = canvas.toBuffer('raw');
    const nerr = [0,0,0];
    const pixel = [0,0,0];
    const error = [0,0,0];
    const infl = [];

    let line = '';

    for( let i=0; i<buf.length; ++i ){
	let j;

	nerr[2] = buf[i++];
	nerr[1] = buf[i++];
	nerr[0] = buf[i++];

	pixel[0] = nerr[0] + error[0];
	pixel[1] = nerr[1] + error[1];
	pixel[2] = nerr[2] + error[2];

	let minind = 0;
	let mindist = (pixel[0]-palette[0][0])*(pixel[0]-palette[0][0]) +
	    (pixel[1]-palette[0][1])*(pixel[1]-palette[0][1]) +
	    (pixel[2]-palette[0][2])*(pixel[2]-palette[0][2]);

	for( j=1; j<palette.length; ++j ){
	    let dist = (pixel[0]-palette[j][0])*(pixel[0]-palette[j][0]) +
		(pixel[1]-palette[j][1])*(pixel[1]-palette[j][1]) +
		(pixel[2]-palette[j][2])*(pixel[2]-palette[j][2]);

	    if( dist < mindist ){
		minind = j;
		mindist = dist;
		if( dist == 0 )
		    break;
	    }
	}
/*
	error[0] = nerr[0] - palette[minind][0];
	error[1] = nerr[1] - palette[minind][1];
	error[2] = nerr[2] - palette[minind][2];
*/
	infl.push( minind );

	line += minind.toString(16);
	if( line.length == w ){
	    console.log(line);
	    line = '';
	}

    }

    const out = [...U32(tagType), ...U32((w>>1)*(h))];

    for( let i=0; i<infl.length; ){
	let hi = infl[i++];
	let lo = infl[i++];
	out.push( (hi<<4) | lo );
    }
    
    return out;
}

let tags = {
    PADDING(amount){
        return [...U32(0)];
    },

    INDEX(){
        return [...U32(1)];
    },

    OFFSETADDRESS( addr ){
        return [
            ...U32(2),
            ...U32(4),
            ...U32(parseInt(addr))
        ];
    },

    CODECHUNK( addr, file ){
        let bin = fs.readFileSync(file);
        return [
            ...U32(3),
            ...U32(bin.length+4),
            ...U32(parseInt(addr)),
            ...bin
        ];
    },

    ENTRYPOINT(){
        return [...U32(4)];
    },

    CRC(){
        return [...U32(5)];
    },

    HAT(){
        return [...U32(6)];
    },

    LONGNAME( name ){
        return [...U32(7), ...STR(name)];
    },

    AUTHOR( name ){
        return [...U32(8), ...STR(name)];
    },

    DESCRIPTION( desc ){
        return [...U32(9), ...STR(desc)];
    },

    IMG_36X36_4BPP( path ){
        return img4bpp( 10, path, 36, 36 );
    },

    IMG_24X24_4BPP( path ){
        return img4bpp( 11, path, 24, 24 );
    },

    IMG_100X24_4BPP( path ){
        return img4bpp( 12, path, 100, 24 );
    },

    IMG_110X88_4BPP( path ){
        return img4bpp( 13, path, 110, 88 );
    },

    CODE( name ){
        return fs.readFileSync(name);
    }    
};

try{
    run();
}catch(ex){
    console.log(ex.message);
}

function run(){
    
    let pop = JSON.parse( fs.readFileSync("pop.json", "utf-8") );

    if( !pop || typeof pop.out != "string" || !Array.isArray(pop.tags) ){
        console.log("pop.json should be in the form: { out:'file', tags:[] }");
        return 3;
    }

    let f = fs.openSync( pop.out, "w" ), pending = 0;

    pop.tags.forEach( ([name, ...args]) => {
        name = name.toUpperCase();
        if( !(name in tags) ){
            console.log("Unknown tag: ", name);
            return;
        }
        
	console.log("tag: " + name);
        let arr = tags[name]( ...args );
	if( arr instanceof Promise ){
	    pending++;

	    arr.then( arr => {
		pending--;
		fs.writeSync( f, Buffer.from(arr) );
		if( !pending )
		    fs.closeSync(f);
	    });

	}else
            fs.writeSync( f, Buffer.from(arr) );
    });

    if( !pending )
	fs.closeSync(f);
    
    return 0;
}
