from SConsCommon import *

# import whatever is given by the parent builder
Import( 'parent_env parent_targets parent_report' )

# OS detection
OSPosix = ( os.name == 'posix' )
OSWindows = ( os.name == 'win32' or sys.platform == 'win32' )

# setup the environment matching the OS
envApp = parent_env.Clone()

libs = []

# do configure stuff
conf = envApp.Configure()
hasFOX = False

# this is a bit messy right now. fox uses hard coded include path which is a
# nuisance par excellence. only recently a fox-config exists but this code is
# based on the old versions which lack such a system. this makes testing for
# fox not an easy thing. currently we stick to a hard code test which is not
# nice and clean but does the job for the time being
for version in [ '1.6', '1.4', '1.3', '1.2' ]:
	if envApp[ 'with_foxstatic' ]:
		lib = 'FOX-%s_static' % version
	else:
		lib = 'FOX-%s' % version
	header = 'fox-%s/fx.h' % version
	hasFOX = conf.CheckLibWithHeader( lib, header, 'c++' )
	if hasFOX:
		foxVersion = version
		envApp.Append( CXXFLAGS = '-DHAS_LIB_FOX' )
		envApp.Append( CXXFLAGS = '-DHAVE_FOX_%s_FX_H' % version.replace( '.', '_' ) )
		libs.append( lib )
		break
if not hasFOX:
	Exit( 'No suitable FOX installation could be found.' )

if OSWindows:
	if envApp[ 'with_foxstatic' ]:
		libs.append( [ 'png_static', 'jpeg_static', 'z_static' ] )
	else:
		libs.append( [ 'png', 'jpeg' ] ) # ,'z'
	libs.append( [ 'kernel32', 'user32', 'gdi32', 'winmm', 'wsock32' ] )
libs.append( [ 'z', 'curl' ] )

conf.Finish()

# write the configuration file required by a few source files
configFilePath = '%s/dmopmconfig.h' % str( Dir( '.' ).srcnode() )
configFileDefines = {}
configFileDefines[ 'OS_UNIX' ] = OSPosix
configFileDefines[ 'OS_W32' ] = OSWindows
for version in [ '1.6', '1.4', '1.3', '1.2' ]:
	configFileDefines[ 'HAVE_FOX_%s_FX_H' % version.replace( '.', '_' ) ] = ( foxVersion == version )

if WriteConfigFile( configFilePath, configFileDefines, envApp ):
	Exit( 'Writing configuration file failed' )

# fetch values in expanded form for later use
pathSConscript = str( Dir( "." ).srcnode() )

pathBin = envApp.subst( envApp[ 'path_bin' ] )

# special environment for minizip, sha1
envMiniZip = envApp.Clone()
envSHA1 = envApp.Clone()

# update the compile flags
if 'CXXFLAGS' in os.environ:
	envApp.Append( CXXFLAGS = os.environ[ 'CXXFLAGS' ] )
	envMiniZip.Append( CXXFLAGS = os.environ[ 'CXXFLAGS' ] )
	envSHA1.Append( CXXFLAGS = os.environ[ 'CXXFLAGS' ] )
envApp.Append( CXXFLAGS = '-O2' )
envMiniZip.Append( CXXFLAGS = '-O2' )
envSHA1.Append( CXXFLAGS = '-O2' )
envApp.Append( CXXFLAGS = '-Wall' )
if envApp[ 'with_warnerrors' ]:
	envApp.Append( CXXFLAGS = '-Werror' )

if 'LDFLAGS' in os.environ:
	envApp.Append( LINKFLAGS = os.environ[ 'LDFLAGS' ] )

# create environment for the progam
if OSWindows:
	envApp.Append( CXXFLAGS = '-DDEBUG_RELOCATE_STDOUT' )
	#envApp.Append( CXXFLAGS = '-DWIN32=1' )
	#envApp.Append( LDFLAGS = '-DWIN32=1' )
	envApp.Append( LINKFLAGS = '-Wl,-subsystem,windows' )

envApp.Append( CXXFLAGS = '-I%s/src' % pathSConscript )
envApp.Append( CXXFLAGS = '-Iminizip' )
envApp.Append( CXXFLAGS = '-Isha1' )

# determine the source files
sources = []

oldcwd = os.getcwd()
os.chdir( str( Dir(".").srcnode() ) )
for root, dirs, files in os.walk( '.' ):
	if '.svn' in dirs:
		dirs.remove( '.svn' )
	for s in fnmatch.filter( files, '*.cpp' ):
		sources.append( root + os.sep + s )
os.chdir( oldcwd )

# setup the builders
objects = []
for s in sources:
	objects.append( envApp.StaticObject( s ) )

objects.append( envMiniZip.StaticObject( '../minizip/unzip.c' ) )
objects.append( envMiniZip.StaticObject( '../minizip/ioapi.c' ) )

objects.append( envSHA1.StaticObject( '../sha1/sha1.cpp' ) )

program = envApp.Program( target='dmopackman', source=objects, LIBS=libs )
targetBuild = envApp.Alias( 'dmopackman_buildonly', program )

install = []
install.append( envApp.Install( pathBin, program ) )
targetInstall = envApp.Alias( 'dmopackman', install )

# add the targets to the targets list
parent_targets[ 'dmopackman' ] = [ 'DMO Pack Manager', targetInstall ]

# add the report entries
if hasFOX:
	parent_report[ 'use FOX ToolKit' ] = 'yes (%s)' % foxVersion
else:
	parent_report[ 'use FOX ToolKit' ] = 'no'
