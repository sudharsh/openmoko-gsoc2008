from string import maketrans

import logging
logger = logging.getLogger('onetworkd')

#============================================================================#
def isValidAddress( address ):
#============================================================================#
    parts = address.split( '.' )
    if len( parts ) != 4:
        return False
    try:
        for part in parts:
            if 0 > int( part ) or 255 < int ( part ):
                return False
        return True
    except ValueError:
        return False

#============================================================================#
def readFromFile( path ):
#============================================================================#
    try:
        value = open( path, 'r' ).read().strip()
    except IOError, e:
        logger.warning( "(could not read from '%s': %s)" % ( path, e ) )
        return "N/A"
    else:
        logger.debug( "(read '%s' from '%s')" % ( value, path ) )
        return value

#============================================================================#
def writeToFile( path, value ):
#============================================================================#
    logger.debug( "(writing '%s' to '%s')" % ( value, path ) )
    try:
        f = open( path, 'w' )
    except IOError, e:
        logger.warning( "(could not write to '%s': %s)" % ( path, e ) )
    else:
        f.write( "%s\n" % value )

#============================================================================#
def cleanObjectName( name ):
#============================================================================#
   return name.translate( trans )

trans = maketrans( "-:", "__" )
