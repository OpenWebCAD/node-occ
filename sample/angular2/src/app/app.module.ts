import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';
import { AceEditorDirective } from 'ng2-ace-editor';
import { LAceEditorModule } from 'angular2-ace';

import { AppComponent } from './app.component';
import { ViewerComponent } from './viewer/viewer.component';
import { EditorComponent } from './editor/editor.component';


@NgModule({
  declarations: [
    AppComponent,
    ViewerComponent,
    EditorComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpModule,
    LAceEditorModule
  ],
  providers: [
    AceEditorDirective
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
