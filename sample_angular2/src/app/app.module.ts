import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';
import { AppComponent } from './app.component';
import { ViewerComponent } from './viewer/viewer.component';
import {GeometryServiceService} from './services/geometry-service.service';

@NgModule({
  declarations: [
    AppComponent,
    ViewerComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpModule
  ],
  providers: [
    GeometryServiceService
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
