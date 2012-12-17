{
  "targets": [
    {
      "target_name": "occ",
	  "msvs_settings": {

				'VCLinkerTool': {					
					'AdditionalLibraryDirectories': [
							"/projects/oce-0.11/Win64/lib/",
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
			"src/Shape.h",
			"src/Shape.cc",
			"src/Mesh.h",
			"src/Mesh.cc",
			"src/Tools.cc",
			"src/OCC.h"
			],
			
	  "defines": [],
	
	  "include_dirs": [  
    	"/projects/oce-0.11/include/oce",
	    "/projet/oce/inc",			    ],				

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
		'TKStepd.lib',
		'TKIGESd.lib',
		],
    }
  ]
}