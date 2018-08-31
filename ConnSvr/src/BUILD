
cc_binary(
	name = 'conn_svr',
	srcs = [
		'main.cc',
		'conn_svr.cc',
		'conn_mgr.cc',
		'conn_client_mgr.cc',

	],

	incs = [
		'../../common/head',
		'../../common/network',
		'../../common/platform',
		'../../common/container',
		'../../common/config',
		'../../common/jybase',
	],

	deps = [
		'//common/platform:platform',	
		'//common/network:network',	
		'//common/container:container',	
		'//common/config:config',	
	],
	
)
