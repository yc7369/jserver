
cc_binary(
	name = 'zone_svr',
	srcs = [
		'main.cc',

		'system/world.cc',
		'framework/zone_config.cc',
	],

	incs = [
		'../../common/base',
		'../../common/config',
		'../../common/platform',

		'../../Zone/src/framework',
		'../../Zone/src/system',
		'../../Zone/src/model',
	
	],

	deps = [
		'//common/config:config',	
		'//common/platform:platform',
	],
	defs = [
	],
)
