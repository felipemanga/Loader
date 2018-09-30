#!/usr/local/bin/nodejs

const fs = require("fs");
const Canvas = require('canvas');
const Image = Canvas.Image;

const palette = // fs.readFileSync('paletteFinal.hex', 'utf-8')
`080408
4a2820
7b5929
bd8d52
eeba73
f6de94
fff6de
bdbaac
73ae4a
73aab4
397d9c
31505a
292041
7b3439
bd695a
e6956a`
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


function U32(n){
    tmpab32[0] = n;
    return tmpab8;
}

function img4bpp( path, w, h ){
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
    
    for( let i=0; i<buf.length; ++i ){
	let j;

	nerr[0] = buf[i++]	
	nerr[1] = buf[i++]	
	nerr[2] = buf[i++]	

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
	    }
	}
/*
	error[0] = nerr[0] - palette[minind][0];
	error[1] = nerr[1] - palette[minind][1];
	error[2] = nerr[2] - palette[minind][2];
*/
	infl.push( minind );

    }

    const out = [...U32(10), ...U32((w>>1)*(h))];

    for( let i=0; i<infl.length; ){
	let hi = infl[i++];
	let lo = infl[i++];
	out.push( (hi<<4) | lo );
    }
    
    return out;
}

let tags = {
    PADDING(){
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

    LONGNAME(){
        return [...U32(7)];
    },

    AUTHOR(){
        return [...U32(8)];
    },

    DESCRIPTION(){
        return [...U32(9)];
    },

    IMG_36X36_4BPP( path ){
        return img4bpp( path, 36, 36 );
    },

    IMG_100X24_4BPP(){
        return [...U32(11)];
    },

    IMG_110X88_4BPP(){
        return [...U32(12)];
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
