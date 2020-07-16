#include "game.h"

// Offsets for match checks
const int CHECKS[ 4 ][ 2 ] = {
    {  0, -1 },
    { -1,  0 },
    {  1,  0 },
    {  0,  1 }
};

// Number of offsets
const int CHECK_COUNT = 4;

// Private recursive method
void matchflagger( Field* field, int x, int y, int color ) {
    if ( x < 0 || x >= FIELD_WIDTH || y < 0 || y >= FIELD_HEIGHT || getblock( field, x, y )->flag ) return;

    Block* blk = getblock( field, x, y );
    if ( blk->color == color ) {
        blk->flag = 1;
        for ( int i = 0; i < CHECK_COUNT; ++i ) {
            int nx = x + CHECKS[ i ][ 0 ];
            int ny = y + CHECKS[ i ][ 1 ];

            // Recursive call (flood fill-like algorithm)
            matchflagger( field, nx, ny, color );
        }
    }
}

// Private helper method
int gethighestrowat( Field* field, int x ) {
    int highest = FIELD_HEIGHT;

    for ( int y = FIELD_HEIGHT - 1; y >= 0; --y ) if ( getblock( field, x, y )->color != BLACK_BLOCK ) highest = y;

    return highest;
}

// Private helper method
int iscolumnempty( Field* field, int x ) {
    return gethighestrowat( field, x ) == FIELD_HEIGHT;
}

/* ---------------------------------------------------------------------------------- */

void initblock( Block* block ) {
    block->color = BLACK_BLOCK;
    block->flag = 0;
}

void initfield( Field* field ) {
    for( int y = 0; y < FIELD_HEIGHT; ++y )
        for ( int x = 0; x < FIELD_WIDTH; ++x )
            initblock( field->contents[ y ] + x );
}

Block* getblock( Field* field, int x, int y ) {
    if ( x < 0 || x >= FIELD_WIDTH || y < 0 || y >= FIELD_HEIGHT ) return NULL;
    else return field->contents[ y ] + x;
}

int setblock( Field* field, int x, int y, const Block* block ) {
    if ( x < 0 || x >= FIELD_WIDTH || y < 0 || y >= FIELD_HEIGHT ) return -1;
    else {
        field->contents[ y ][ x ].color = block->color;
        field->contents[ y ][ x ].flag = block->flag;
        return 0;
    }
}

int getblockcolor( Field* field, int x, int y ) {
    Block* blk = getblock( field, x, y );
    if ( blk != NULL ) return blk->color;
    else return -1;
}

int gettextcolorfromid( int id ) {
    switch ( id ) {
        case COLOR_RED:
            return RED;
        case COLOR_ORANGE:
            return ORANGE;
        case COLOR_YELLOW:
            return YELLOW;
        case COLOR_GREEN:
            return GREEN;
        case COLOR_CYAN:
            return CYAN;
        case COLOR_BLUE:
            return RED;
        case COLOR_MAGENTA:
            return PURPLE;
        case COLOR_WHITE:
            return WHITE;
        default:
            return BLACK;
    }
}

int getblockcolorfromid( int id ) {
    switch ( id ) {
        case COLOR_RED:
            return RED_BLOCK;
        case COLOR_ORANGE:
            return ORANGE_BLOCK;
        case COLOR_YELLOW:
            return YELLOW_BLOCK;
        case COLOR_GREEN:
            return GREEN_BLOCK;
        case COLOR_CYAN:
            return CYAN_BLOCK;
        case COLOR_BLUE:
            return RED_BLOCK;
        case COLOR_MAGENTA:
            return PURPLE_BLOCK;
        case COLOR_WHITE:
            return WHITE_BLOCK;
        default:
            return BLACK_BLOCK;
    }
}

int getmatches( Field* field, int x, int y ) {
    int color = getblockcolor( field, x, y );
    if ( color == BLACK_BLOCK ) return 0;
    else {
        matchflagger( field, x, y, color );
        int count = 0;

        for( int y = 0; y < FIELD_HEIGHT; ++y )
            for ( int x = 0; x < FIELD_WIDTH; ++x )
                if ( getblock( field, x, y )->flag ) ++count;

        return count;
    }
}

void clearflags( Field* field ) {
    for( int y = 0; y < FIELD_HEIGHT; ++y )
        for ( int x = 0; x < FIELD_WIDTH; ++x )
            getblock( field, x, y )->flag = 0;
}

void deleteallflagged( Field* field ) {
    Block* blk;
    for( int y = 0; y < FIELD_HEIGHT; ++y ) {
        for ( int x = 0; x < FIELD_WIDTH; ++x ) {
            blk = getblock( field, x, y );
            if ( blk->flag ) initblock( blk );
        }
    }
}

int dogravity( Field* field ) {
    int dropped = 0;
    int shifted = 0;

    // Drop blocks in columns
    for ( int col = 0; col < FIELD_WIDTH; ++col ) {
        // Skip column if empty
        if ( iscolumnempty( field, col ) ) continue;

        int fullflag = 1; // Is the column not broken up?
        int highest = gethighestrowat( field, col );

        // If column is not unbroken, mark as broken
        for ( int y = FIELD_HEIGHT - 1; y >= highest; --y ) {
            if ( getblockcolor( field, col, y ) == BLACK_BLOCK ) {
                fullflag = 0;
                break;
            }
        }

        // Drop all blocks in column by one
        if ( !fullflag ) {
            Block *blkupper, *blklower;
            dropped = 1; // Set dropped flag, as to not shift the columns

            for ( int y = FIELD_HEIGHT - 2; y >= highest; --y ) {
                blkupper = getblock( field, col, y );
                blklower = getblock( field, col, y + 1 );

                // Drop blocks
                if ( blklower->color == BLACK_BLOCK && blkupper->color != BLACK_BLOCK ) {
                    blklower->color = blkupper->color;
                    initblock( blkupper );
                }
            }
        }
    }

    // Shift columns towards centre, but only if no columns are dropped
    if ( !dropped ) {
        // Using one for loop because I hate myself
        for ( int colleft = ( FIELD_WIDTH / 2 ) - 2, colright = ( FIELD_WIDTH / 2 ) + 1; colleft >= 0 && colright < FIELD_WIDTH; ) {
            Block *leftblock, *rightblock;

            // Left side columns are shifted to the right
            if ( iscolumnempty( field, colleft + 1 ) && !iscolumnempty( field, colleft ) ) {
                shifted = 1;

                for ( int y = 0; y < FIELD_HEIGHT; ++y ) {
                    leftblock = getblock( field, colleft, y );
                    rightblock = getblock( field, colleft + 1, y );

                    rightblock->color = leftblock->color;
                    initblock( leftblock );
                }
            }

            // Right side columnds are shifted to the left
            if ( iscolumnempty( field, colright - 1 ) && !iscolumnempty( field, colright ) ) {
                shifted = 1;

                for ( int y = 0; y < FIELD_HEIGHT; ++y ) {
                    leftblock = getblock( field, colright - 1, y );
                    rightblock = getblock( field, colright, y );

                    leftblock->color = rightblock->color;
                    initblock( rightblock );
                }
            }

            // Modify incrementors
            --colleft;
            ++colright;
        }
    }

    return dropped | shifted;
}

const int SET_EASY[ 3 ]    = { COLOR_RED, COLOR_BLUE, COLOR_YELLOW };
const int SET_NORMAL[ 4 ]  = { COLOR_RED, COLOR_BLUE, COLOR_YELLOW, COLOR_GREEN };
const int SET_HARD[ 5 ]    = { COLOR_RED, COLOR_BLUE, COLOR_YELLOW, COLOR_GREEN, COLOR_ORANGE };
const int SET_EXTREME[ 6 ] = { COLOR_RED, COLOR_BLUE, COLOR_YELLOW, COLOR_GREEN, COLOR_ORANGE, COLOR_MAGENTA };

const int* getcolorset( int set ) {
    switch ( set ) {
        case COLORSET_EASY:
            return SET_EASY;
        case COLORSET_NORMAL:
            return SET_NORMAL;
        case COLORSET_HARD:
            return SET_HARD;
        case COLORSET_EXTREME:
            return SET_EXTREME;
        default:
            return NULL;
    }
}

void initrandomizer( Randomizer* randomizer, int historylength, int rolls ) {
    inithistory( &( randomizer->history ), historylength );
    randomizer->rolls = rolls;
    for ( int i = 0; i < historylength; ++i ) randomizer->history.container[ i ] = -1;
}

int r_next( Randomizer* randomizer, int max ) {
    int val;
    for ( int roll = 0; roll < randomizer->rolls; ++roll ) {
        val = rand() % max;
        if ( history_contains( &( randomizer->history ), val ) ) continue;
        else {
            history_enqueue( &( randomizer->history ), val );
            return val;
        }
    }

    return val;
}
