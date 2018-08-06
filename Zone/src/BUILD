
cc_binary(
	name = 'zone_svr',
	srcs = [
		'main.cc',

		'system/world.cc',
		'framework/zone_config.cc',
	],

	incs = [
		'../../Zone/src/framework',
		'../../Zone/src/system',
		'../../Zone/src/model',

		'../../common/base',
		'../../common/config',
		'../../common/platform',
		'../../common/jybase',
	
	],

	deps = [
		'//common/config:config',	
		'//common/platform:platform',
		'//common/jybase:jybase',	
		'//common/base:run_flags',	
	],
	defs = [
	],
)
