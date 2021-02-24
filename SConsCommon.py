# -*- coding: utf-8 -*-
# Taken from Drag[en]gine Game Engine
# http://www.moddb.com/engines/dragengine
# http://dragengine.rptd.ch

import os
import sys
import string
import fnmatch
import subprocess
import hashlib



# ternary variable
_TernaryVariableStringsYes = ( 'y', 'yes', 'true', 't', '1', 'on', 'all' )
_TernaryVariableStringsNo = ( 'n', 'no', 'false', 'f', '0', 'off', 'none' )
_TernaryVariableStringsAuto = ( 'auto' )

TernaryVariableYes = 'yes'
TernaryVariableNo = 'no'
TernaryVariableAuto = 'auto'

def _TernaryVariableMapper( text ):
	textLower = string.lower( text )
	if textLower in _TernaryVariableStringsYes:
		return TernaryVariableYes
	if textLower in _TernaryVariableStringsNo:
		return TernaryVariableNo
	if textLower in _TernaryVariableStringsAuto:
		return TernaryVariableAuto
	raise ValueError( 'Invalid value for ternary option: %s' % text )

def _TernaryVariableValidator( key, value, env ):
	if not env[ key ] in ( TernaryVariableYes, TernaryVariableNo, TernaryVariableAuto ):
		raise SCons.Errors.UserError( 'Invalid value for ternary option %s: %s' % ( key, env[ key ] ) )

def TernaryVariable( key, help, default=TernaryVariableAuto ):
	return ( key, '%s (yes|no|auto)' % help, default, _TernaryVariableValidator, _TernaryVariableMapper )



# library symbolic link action
def SymLinkLibrary( env, target, source ):
	os.symlink( str( source[ 0 ] ).rsplit( os.sep, 1 )[ 1 ], str( target[ 0 ] ) )
	return 0 # how does os.symlink signal an error?



# write configuration file action
def WriteConfigFile( target, defines, env ):
	file = open( target, 'w' )
	
	file.write( '/********************************************+\n' )
	file.write( '|| Configuration File. Do not edit since     |\n' )
	file.write( '|| this is a generated file and all changes  |\n' )
	file.write( '|| will be lost the next time scons runs.    |\n' )
	file.write( '`********************************************/\n' )
	file.write( '\n' )
	
	for define in defines:
		if defines[ define ]:
			file.write( '/* enable %s */\n' % define )
			file.write( '#ifndef %s\n' % define )
			file.write( '#define %s 1\n' % define )
		else:
			file.write( '/* disable %s */\n' % define )
			file.write( '#ifdef %s\n' % define )
			file.write( '#undef %s\n' % define )
		file.write( '#endif\n' )
		file.write( '\n' )
	
	file.write( '/* End of configuration file. */\n' )
	file.close()
	
	return 0



# windows sucks hack ( http://www.scons.org/wiki/LongCmdLinesOnWin32 )
def WindowsSpawnHack( envW32 ):
	class ourSpawn:
		def ourspawn(self, sh, escape, cmd, args, env):
			newargs = string.join(args[1:], ' ')
			cmdline = cmd + " " + newargs
			startupinfo = subprocess.STARTUPINFO()
			startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
			proc = subprocess.Popen(cmdline, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
				stderr=subprocess.PIPE, startupinfo=startupinfo, shell = False, env = env)
			data, err = proc.communicate()
			rv = proc.wait()
			if rv:
				print "====="
				print err
				print "====="
			return rv
	
	buf = ourSpawn()
	buf.ourenv = envW32
	envW32['SPAWN'] = buf.ourspawn
