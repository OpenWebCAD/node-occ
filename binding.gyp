{
  "targets": [
    {
      "target_name": "occ",
	  "msvs_settings": {

				'VCLinkerTool': {					
					'AdditionalLibraryDirectories': [
							#"c:/Program Files (x86)/boost/boost_1_46_1/lib",
							"/projet/oce-build/bin/Debug/",
					],
					# 'AdditionalOptions': ["/FORCE",],						
				},	  
	  },
		
      "sources": [
			"src/GeometryBuilder.cc",
			"src/Transform.cc",
			"src/Util.cc",
			"src/V8Wrapper.cc",
			"src/Util.h",
			"src/Transform.h",
			"src/GeometryBuilder.h",
			"src/NodeV8.h",
			"src/Transformation.h",
			"src/Transformation.cc",
			"src/Solid.h",
			"src/Solid.cc",
			"src/Mesh.h",
			"src/Mesh.cc",
			"src/OCC.h"
			],
			
	  "defines": [],
	
	  "include_dirs": [  "/projet/oce/inc",			    ],				

	"libraries": [ 
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