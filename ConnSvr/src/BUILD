
cc_binary(
	name = 'conn_svr',
	srcs = [
		'main.cc',
		'conn_svr.cc',
		'conn_mgr.cc',
		'conn_client_mgr.cc',
		'conn_svr_wrapper.cc',
	],

	incs = [
		'../../common/head',
		'../../common/network',
		'../../common/platform',
		'../../common/container',
		'../../common/config',
		'../../common/jybase',
		'../../build64_release/Proto',
	],

	deps = [
		'//common/platform:platform',	
		'//common/network:network',	
		'//common/container:container',	
		'//common/config:config',	
		'//Proto:pb_proto',
		'#protobuf'
	],
	
)
