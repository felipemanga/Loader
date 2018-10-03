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

fs.writeFileSync(
    "../BUILD/wallpaper.16c",
    Buffer.from( img4bpp( "snow.png", 110, 88 ) )    
);

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

    const out = [];

    for( let i=0; i<infl.length; ){
	let hi = infl[i++];
	let lo = infl[i++];
	out.push( (hi<<4) | lo );
    }
    
    return out;
}

