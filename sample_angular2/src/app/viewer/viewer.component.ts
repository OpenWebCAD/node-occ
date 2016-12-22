/// <reference path="../../typings/three.d.ts" />
/// <reference path="../../typings/three-orbitcontrols.d.ts" />

import { Component, OnInit, ElementRef, Input, SimpleChanges, OnChanges } from '@angular/core';

import { Occ } from "../../model/document";
import { GeometryServiceService } from "../services/geometry-service.service";
import OCC_Document = Occ.OCC_Document;

@Component({
  selector: 'app-viewer',
  templateUrl: 'viewer.component.html',
  styleUrls: ['./viewer.component.css']
})
export class ViewerComponent implements OnInit, OnChanges {

  @Input hiddenSideBar: boolean;

  hostElement: ElementRef;

  constructor(el: ElementRef, private geometryService: GeometryServiceService) {
    this.hostElement = el;
  }

  ngOnInit() {
    document.body.className = "viewer-page";
    this.geometryService.setControl(this.hostElement);
  }

  ngOnChanges(changes: SimpleChanges) {
    console.log("resize");
    this.geometryService.setControl(this.hostElement);
    // changes.prop contains the old and the new value...
  }


  ngOnChange(){

  }
}
