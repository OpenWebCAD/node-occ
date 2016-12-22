import { Injectable, ElementRef } from '@angular/core';
import { Http, Headers } from "@angular/http";
import { Observable } from "rxjs";
import OCC_Document = Occ.OCC_Document;
import { Occ } from "../../model/document";


@Injectable()
export class GeometryServiceService {
  get doc(): Occ.OCC_Document {
    return this._doc;
  }

  set doc(value: Occ.OCC_Document) {
    this._doc = value;
  }

  private _url = 'http://localhost:3000/csg1';

  constructor(private http: Http) { }
  private _doc: OCC_Document;
  setControl(elem: ElementRef )
  {
   this._doc = new OCC_Document(elem);
  }

  resizeViewport()
  {
    this._doc.resizeViewport();
  }

  postScript(script: string) {

    var encoded_script = encodeURIComponent(script);

    var headers = new Headers();
    headers.append('Content-Type', 'application/x-www-form-urlencoded');

    let data = "script=" + encoded_script;

    return Observable.create(observer => {
      this.http.post(this._url, data, {
        headers: headers
      })
        .map(res => res.json())
        .subscribe(
          data => {
            observer.next(data);
            observer.complete();
          },
          err => {
            console.error(err);
            observer.error(err);
            observer.complete();
          },
          () => {
            observer.complete();
          }
        )
    });
  }



}
