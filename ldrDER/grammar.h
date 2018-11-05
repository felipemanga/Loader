#pragma once

#include "div.h"

uint32_t rndState = 0x12345678, depth;
uint32_t rnd(){
    rndState ^= rndState<<17;
    rndState ^= rndState>>13;
    rndState ^= rndState<<5;
    return rndState;
}

uint32_t rnd( uint32_t min, uint32_t max ){
    return MODULO(rnd(), max - min) + min;
}

using Rule = void (*)( uint32_t x, uint32_t y );

extern Rule root;

template<uint32_t C> void apply( Rule (&rules)[C], uint32_t x, uint32_t y  ){
    depth++;
    rules[ MODULO( rnd(), C ) ]( x, y );
    depth--;
}

void apply( Rule rule, uint32_t x, uint32_t y ){
    depth++;
    rule( x, y );
    depth--;
}

void initGrammar( uint32_t seed, uint32_t startX, uint32_t startY ){
    rndState = seed;
    apply( root, startX, startY );
}

#define RULE(name) Rule name[] = 
