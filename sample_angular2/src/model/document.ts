/**
 * Created by victofel based on a project of etienne.rossignon@gadz.org
 */

import { ElementRef } from '@angular/core';

export module Occ {

  export class OCC_Document {

    renderer: THREE.WebGLRenderer;
    scene: THREE.Scene;
    camera: THREE.PerspectiveCamera;
    hostElement: ElementRef;
    grid: THREE.GridHelper;
    mousex: number;
    mousey: number;
    lightContainer: THREE.Object3D;
    controls: THREE.TrackballControls;

    fc: any;

    constructor(el: ElementRef) {

      this.hostElement = el;
      this.init();
    }

    init() {
      this.scene = new THREE.Scene();
      console.log("Init scene", this.hostElement);

      var width = window.innerWidth - document.getElementById('sidebar-wrapper').clientWidth;
      var height = document.getElementById('sidebar-wrapper').clientHeight - document.getElementById('navbar').clientHeight;

      this.camera = new THREE.PerspectiveCamera(35, width / height, 0.01, 100000);
      this.camera.aspect = width / height;
      this.camera.position.z = 100;

      this.renderer = new THREE.WebGLRenderer({ alpha: true, antialias: true });

      this.renderer.setSize(width, height);
      this.hostElement.nativeElement.appendChild(this.renderer.domElement);
      this.addGrid(250, 25);


      this.lightContainer = new THREE.Object3D();
      this.lightContainer.matrixWorld = this.camera.matrix;
      this.lightContainer.matrixAutoUpdate = false;

      this.scene.add(this.lightContainer);

      for (var x = -1; x < 2; x = x + 2) {
        for (var y = -1; y < 2; y = y + 2) {
          for (var z = -1; z < 2; z = z + 2) {

            var pointLight = new THREE.PointLight(0xFFFFFF, 0.4);
            pointLight.position.x = 100 * x;
            pointLight.position.y = 100 * y;
            pointLight.position.z = 100 * z;
            pointLight.matrixAutoUpdate = true;

            this.lightContainer.add(pointLight);
            this.lightContainer.add(new THREE.PointLightHelper(pointLight, 10));
          }
        }
      }

      this.controls = new THREE.TrackballControls(this.camera);

      this.controls.rotateSpeed = 4.0;
      this.controls.zoomSpeed = 1.2;
      this.controls.panSpeed = 0.8;

      this.controls.noZoom = false;
      this.controls.noPan = false;

      this.controls.staticMoving = true;
      this.controls.dynamicDampingFactor = 0.3;

      this.controls.addEventListener('change', function () {
        this.renderer.render(this.scene, this.camera);
      });

      var radius = 1.0;
      this.controls.minDistance = radius * 1.1;
      this.controls.maxDistance = radius * 10000;

      this.controls.keys = [/*A*/65, /*S*/ 83, /*D*/68];

      // handle window resize
      window.addEventListener('resize', () => {

        var wrap = document.getElementById('wrapper');
        var sidebarHidden = wrap.className == 'active';

        var width = sidebarHidden ? window.innerWidth : window.innerWidth - document.getElementById('sidebar-wrapper').clientWidth;
        var height = document.getElementById('sidebar-wrapper').clientHeight - document.getElementById('navbar').clientHeight;

        this.renderer.setSize(width, height);
        this.camera.aspect = width / height;
        this.camera.updateProjectionMatrix();
      }, false);

      var onRenderFcts = [];

      // render the scene
      onRenderFcts.push(() => {
        this.renderer.render(this.scene, this.camera);
        if (this.controls !== null) this.controls.update();
      });

      // run the rendering loop
      var lastTimeMsec = null;
      requestAnimationFrame(function animate(nowMsec) {
        // keep looping
        requestAnimationFrame(animate);
        // measure time
        lastTimeMsec = lastTimeMsec || nowMsec - 1000 / 60;
        var deltaMsec = Math.min(200, nowMsec - lastTimeMsec);
        lastTimeMsec = nowMsec;

        // call each update function
        onRenderFcts.forEach(function (onRenderFct) {
          onRenderFct(deltaMsec / 1000, nowMsec / 1000)
        });

      });

    }

    addGrid(size: number, step: number) {

      this.scene.add(new THREE.AxisHelper(size));

      // Create a grid of size, using step width
      var gridGeometry: THREE.Geometry = new THREE.Geometry();
      var gridMaterial: THREE.LineBasicMaterial = new THREE.LineBasicMaterial({
        color: 0x444444,
        opacity: 1
      })


      for (var s = -size; s <= size; s += step) {

        if (s !== 0) {
          gridGeometry.vertices.push(new THREE.Vector3(s, -size, 0));
          gridGeometry.vertices.push(new THREE.Vector3(s, size, 0));

          gridGeometry.vertices.push(new THREE.Vector3(-size, s, 0));
          gridGeometry.vertices.push(new THREE.Vector3(size, s, 0));
        }
      }

      gridGeometry.vertices.push(new THREE.Vector3(-size, 0, 0));
      gridGeometry.vertices.push(new THREE.Vector3(0, 0, 0));
      gridGeometry.vertices.push(new THREE.Vector3(0, -size, 0));
      gridGeometry.vertices.push(new THREE.Vector3(0, 0, 0));


      var grid = new THREE.LineSegments(gridGeometry, gridMaterial);
      this.scene.add(grid);
      this.onSceneChange();
    }

    resizeViewport() {
      var wrap = document.getElementById('wrapper');
      var sidebarHidden = wrap.className == 'active';

      var width = sidebarHidden ? window.innerWidth - document.getElementById('sidebar-wrapper').clientWidth : window.innerWidth;
      var height = document.getElementById('sidebar-wrapper').clientHeight - document.getElementById('navbar').clientHeight;

      this.renderer.setSize(width, height);
      this.camera.aspect = width / height;
      this.camera.updateProjectionMatrix();

    }

    onMouseMove(event: MouseEvent) {
      this.mousex = event.offsetX;
      this.mousey = event.offsetY;
    }

    onSceneChange() {
      this.camera.lookAt(this.scene.position);
      this.renderer.render(this.scene, this.camera);
    }

    zoomExtens() {
      var rootNode = this.scene.getObjectByName("SOLIDS");
      // Busquem la bounding
      var bbox = new THREE.Box3();
      if (rootNode instanceof THREE.Mesh) {
        var geometry = rootNode.geometry;
        geometry.computeBoundingBox();
        bbox = geometry.boundingBox;
      }

      if (rootNode instanceof THREE.Object3D) {
        for (var i = 0; i < rootNode.children.length; i++) {
          var obj = rootNode.children[i];
          if (obj instanceof THREE.Mesh) {
            var geometryLocal = obj.geometry;
            geometryLocal.computeBoundingBox();
            bbox.union(geometryLocal.boundingBox);
          }

        }
      }

      if (bbox.isEmpty()) {
        console.log("bbox is empty");
        return;
      }
      // Get center
      var COG = bbox.getCenter();

      if (rootNode instanceof THREE.Vector3) {
        COG = rootNode;
      }

      var v = new THREE.Vector3();
      v.subVectors(COG, this.controls.target);
      this.camera.position.addVectors(this.camera.position, v);

      // retrieve camera orientation
      this.camera.lookAt(COG);
      this.controls.target.set(COG.x, COG.y, COG.z);

      var sphereSize = bbox.getSize().length() * 0.5;
      var distToCenter = sphereSize / Math.sin(Math.PI / 180.0 * this.camera.fov * 0.5);
      // move the camera backward
      var target = this.controls.target;
      var vec = new THREE.Vector3();
      vec.subVectors(this.camera.position, target);
      vec.setLength(distToCenter);
      this.camera.position.addVectors(vec, target);
      this.camera.updateProjectionMatrix();

      //  this.scene.updateMatrixWorld(true);//.updateMatrixWorld();

      this.renderer.render(this.scene, this.camera);
      console.log("fi");

    }

    /**
     *  Convert a rgb color to hex,
     *  each Red Green Blue component of RGB shall be in the range [0,1]
     *
     */
    rgb2hex(rgb) {
      return ( rgb[0] * 255 << 16 ) + ( rgb[1] * 255 << 8 ) + rgb[2] * 255;
    }

    new() {
      var rootNode = this.scene.getObjectByName("SOLIDS");
      if (rootNode) {
        this.scene.remove(rootNode);
      }
    }

    /*
     *  json = { solids: [ id: "ID" , { faces: [], edges: [] }, ...]}
     *
     *
     */
    updateShapeObject(json, next) {

      var rootNode = this.scene.getObjectByName("SOLIDS");
      if (!rootNode) {
        rootNode = new THREE.Object3D();
        rootNode.name = "SOLIDS";
        this.scene.add(rootNode);
      }

      var node = rootNode;
      if (json.name) {
        var oldObj = rootNode.getObjectByName(json.name);
        if (oldObj) {
          rootNode.remove(oldObj);
        }
        node = new THREE.Object3D();
        node.name = json.name;
        rootNode.add(node);
      }

      // read solids
      var jsonSolids = json.solids;
      jsonSolids.forEach(function (solidMesh) {

        var color = [Math.random(), Math.random(), Math.random()];

        if (solidMesh.color instanceof Array) {
          color = solidMesh.color.slice(0);
        }
        var group = new THREE.Object3D();
        node.add(group);
        if (undefined == group.properties) {
          group.properties = {};
        }
        group.properties.OCCType = "Solid";
        group.properties.OCCName = solidMesh.name;
        group.properties.OCCID = solidMesh.id;
        group.properties.OCCColor = color;


        // one object
        for (let face of solidMesh.faces) {
          var jsonFace = face.mesh;

          jsonFace.scale = 1.0;
          var jsonLoader = new THREE.JSONLoader();

          var model;

          model = jsonLoader.parse(jsonFace, /* texture path */undefined);

          var material = new THREE.MeshLambertMaterial({ color: ( color[0] * 255 << 16 ) + ( color[1] * 255 << 8 ) + color[2] * 255 });
          var mesh = new THREE.Mesh(model.geometry, material);
          mesh.properties = {};
          mesh.properties.OCCType = "face";
          mesh.properties.OCCName = jsonFace.name;
          rootNode.add(mesh);
          //this.process_face_mesh(group, face, color);
        }

        for (let edge of solidMesh.edges) {
          var v = edge.mesh;
          var geometry = new THREE.Geometry();
          var i = 0
          while (i < v.length) {
            geometry.vertices.push(new THREE.Vector3(v[i], v[i + 1], v[i + 2]));
            i += 3;
          }
          var materialLine = new THREE.LineDashedMaterial({ linewidth: 2, color: 0xffffff });
          var polyline = new THREE.Line(geometry, materialLine);
          polyline.properties = {};
          polyline.properties.OCCType = "edge";
          polyline.properties.OCCName = edge.name;
          rootNode.add(polyline);

        }

      });
    };

    clearAll() {
      var rootNode = this.scene.getObjectByName("SOLIDS");
      if (!rootNode) {
        var rootNode = new THREE.Object3D();
        rootNode.name = "SOLIDS";
        this.scene.add(rootNode);
      }
      if (rootNode) {
        this.scene.remove(rootNode);
      }
    };
  }
}
