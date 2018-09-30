#!/usr/local/bin/nodejs

const fs = require("fs");

const pal888 = fs.readFileSync('palette.hex', 'utf-8')
      .split('\n')
      .filter( x => !!x.length )
      .map( x => parseInt(x, 16) );

const pal565 = pal888.map( c => (
    ((c>>>16)/0xFF*0x1F<<11) +
    (((c>>>8)&0xFF)/0xFF*0x3F<<5) +
      ((c&0xFF)/0xFF*0x1F>>>0) 
) );


const palette = pal565.map( c => (
    ((c>>11)/0x1F*0xFF>>>0<<16) +
        (((c>>5)&0x3F)/0x3F*0xFF>>>0<<8) +
        ((c&0x1F)/0x1F*0xFF>>>0)
));

fs.writeFileSync(
    'palette565.hex',
    pal565.map( c => '0x' + c.toString(16).padStart(4,'0') ).join(',\n'),
    'utf-8'
);
fs.writeFileSync(
    'paletteFinal.hex',
    palette.map( c => c.toString(16).padStart(6,'0') ).join('\n'),
    'utf-8'
);
