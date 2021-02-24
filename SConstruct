from SConsCommon import *

# OS detection
print 'os.name', os.name
print 'sys.platform', sys.platform
OSPosix = ( os.name == 'posix' )
OSWindows = ( os.name == 'win32' or sys.platform == 'win32' )

# create environment
if len( ARGUMENTS.get( 'tools', '' ) ) > 0:
    parent_env = Environment( CPPPATH='.', LIBPATH='.', tools=[ARGUMENTS.get( 'tools', '' )] )
    
else:
    parent_env = Environment( CPPPATH='.', LIBPATH='.' )

if OSPosix:
	parent_env.Append( CXXFLAGS = '-DOS_UNIX' )
	
if OSWindows:
	parent_env.Append( CXXFLAGS = '-DOS_W32' )
	
if not ( OSPosix or OSWindows ):
	Exit( 'No supported OS found!')

# parameters
#params = Variables( [ 'parameters.cache', 'custom.py' ] )
params = Variables( [ 'custom.py' ] )
#params.Add( TernaryVariable( 'with_tests', 'Build engine tests', TernaryVariableNo ) )
params.Add( BoolVariable( 'with_warnerrors', 'Treat warnings as errors ( dev-builds )', False ) )
params.Add( BoolVariable( 'with_foxstatic', 'Link FOX library statically', True ) )

if OSPosix:
	params.Add( PathVariable( 'prefix', 'System path', '/usr', PathVariable.PathAccept ) )
	params.Add( PathVariable( 'sysconfdir', 'System configuration', '/etc', PathVariable.PathAccept ) )
	params.Add( PathVariable( 'execdir', 'System binaries', '${prefix}/bin', PathVariable.PathAccept ) )
	
	params.Add( PathVariable( 'path_bin', 'Path to the DMO Pack Manager binaries',
		'${execdir}', PathVariable.PathAccept ) )
	
elif OSWindows:
	params.Add( PathVariable( 'path_bin', 'Path to the DMO Pack Manager Installation',
		'${PROGRAMFILES}\DMO Pack Manager', PathVariable.PathAccept ) )

params.Update( parent_env )

# no default targets
Default( None )

# define the targets array and reports dictionary to be filled
parent_targets = {}
parent_report = {}

# report stuff
if OSPosix:
	parent_report[ 'prefix' ] = parent_env.subst( parent_env[ 'prefix' ] )
elif OSWindows:
	parent_report[ 'dmo pack manager path' ] = parent_env.subst( parent_env[ 'path_bin' ] )
#parent_report[ 'build dragengine tests' ] = parent_env.subst( parent_env[ 'with_tests' ] )
if parent_env[ 'with_warnerrors' ]:
	parent_report[ 'treat warnings as errors' ] = 'yes'
else:
	parent_report[ 'treat warnings as errors' ] = 'no'

SConscript( dirs='src', variant_dir='build', duplicate=0, exports='parent_env parent_targets parent_report' )

#params.Save( 'parameters.cache', parent_env )

# produce help
maxLenTarget = 1
maxLenDescription = 1

keys = parent_targets.keys()
keys.sort()
for key in keys:
	lenTarget = len( key )
	lenDescription = len( parent_targets[ key ][ 0 ] )
	if lenTarget > maxLenTarget:
		maxLenTarget = lenTarget
	if lenDescription > maxLenDescription:
		maxLenDescription = lenDescription

helpText = [ """
scons dmopackman:
	Build and install the application
scons -c dmopackman:
	Clear and uninstall target
""" ]

helpText.append( ( '%s | Description\n' % 'Target'.ljust( maxLenTarget ) ) )
helpText.append( ( '%s-+-%s\n' % ( ''.ljust( maxLenTarget, '-' ), ''.ljust( maxLenDescription, '-' ) ) ) )

for key in keys:
	helpText.append( '%s | %s\n' % ( key.ljust( maxLenTarget ), parent_targets[ key ][ 0 ] ) )
helpText.append( '\n' )
Help( ''.join( helpText ) )

# print out report
maxLenKey = 1
maxLenValue = 1

keys = parent_report.keys()
keys.sort()
for key in keys:
	lenKey = len( key )
	lenValue = len( parent_report[ key ] )
	if lenKey > maxLenKey:
		maxLenKey = lenKey
	if lenValue > maxLenValue:
		maxLenValue = lenValue

print ''
print 'Configuration Report:'
print '%s-+-%s' % ( ''.ljust( maxLenKey, '-' ), ''.ljust( maxLenValue, '-' ) )
for key in keys:
	print '%s | %s' % ( key.ljust( maxLenKey ), parent_report[ key ] )
print ''
