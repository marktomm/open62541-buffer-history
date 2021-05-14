
project('opcua-test', 'cpp', 
    default_options : ['cpp_std=c++11'],
	version : '1.0.0')

open62541dep = dependency('open62541')

client_history_sources = [ 'client_history.cpp' ]

open62541_client = executable('client-history', 
	client_history_sources, 
	dependencies: [ 
		open62541dep
	]
)

inc = include_directories( 'includes' )

server_history_custom_sources = [ 'server_history_custom.cpp' ]

open62541_server_history_custom = executable('server-history-custom', 
	server_history_custom_sources, 
  	include_directories:inc,
	dependencies: [ 
		open62541dep
	]
)