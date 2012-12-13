{
  "targets": [
    {
      "target_name": "binding",
	  
	"conditions": [
          [		'OS=="win"', {

			'msvs_settings': {

				'VCCLCompilerTool': {
					#'ExceptionHandling': '1', 
					#'ExceptionHandling': '2',
					# 'WarnAsError': 'true',
					'AdditionalOptions': [
						"/bigobj",
					],					
				},
				'VCLibrarianTool': {
						'AdditionalOptions': [
						],
				},
				'VCLinkerTool': {					
					'AdditionalLibraryDirectories': [
							"c:/Program Files (x86)/boost/boost_1_46_1/lib",
							"/projet/oce-build/bin/Debug/",
					],
					'AdditionalOptions': [
						"/FORCE",
					],						
					'ForceFileOutput' : 'MultiplyDefinedSymbolOnly',
				},
			
}		  
		  }, 
				{
					# OS != "win"
					
				}
		  ],
        ],

		
      "sources": [
			"src/GeometryBuilder.cc",
			"src/Transform.cc",
			"src/Util.cc",
			"src/V8Wrapper.cc",
			"deps/json_spirit_v4.05/json_spirit/json_spirit_reader.cpp",
			"deps/json_spirit_v4.05/json_spirit/json_spirit_value.cpp",
			"deps/json_spirit_v4.05/json_spirit/json_spirit_writer.cpp",
			],
			
	  "defines": [		"JSON_SPIRIT_STATIC_LINKED"],
	
	  "include_dirs": [ 
			"d:/projet/oce/inc",
			"deps/json_spirit_v4.05",
			"c:/Program Files (x86)/boost/boost_1_46_1/"
			
	
	],				
	"libraries_dir": [
				"c:/Program Files (x86)/boost/boost_1_46_1/lib",
				"/projet/oce-build/bin/Debug/"    
		],
	 "libraries": [ 
		"libboost_system-vc100-mt-gd-1_46_1.lib",
		"libboost_thread-vc100-mt-gd-1_46_1.lib",	
		"libboost_signals-vc100-mt-gd-1_46_1.lib",	
		'TKAdvToolsd.lib',
		'TKBOd.lib',
		'TKBoold.lib',
		'TKBRepd.lib',
		'TKerneld.lib',
		'TKFeatd.lib',
		'TKFilletd.lib',
		'TKG2dd.lib',
		'TKG3dd.lib',
		'TKGeomAlgod.lib',
		'TKGeomBased.lib',
		'TKHLRd.lib',
		'TKMathd.lib',
		'TKMeshd.lib',
		'TKOffsetd.lib',
		'TKPrimd.lib',
		'TKShHealingd.lib',
		'TKTopAlgod.lib',
		'TKXMeshd.lib',
		],
    }
  ]
}