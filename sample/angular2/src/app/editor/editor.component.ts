import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-editor',
  templateUrl: './editor.component.html',
  styleUrls: ['./editor.component.css']
})
export class EditorComponent implements OnInit {

  private aceOption: any;
  private mode: string = "text";
  private editValue: string = "hello";

  constructor() { }

  ngOnInit() {
    this.aceOption = {
      readonly: false,
      theme: 'twilight',
      onLoaded: (editor) => {
        editor.$blockScrolling = Infinity
        editor.setOptions({
          minLines: 15,
          maxLines: 25
        })
      },
      onChange: (e) => {
        console.log('sale algo');
      }
    }
  }

}

export interface IAceEditorOption {
  readonly: boolean;
  theme: string;
  fontSize: number;
  tabSize: number;
  enableEmmet: boolean;
  enableSnippets: boolean;
  showPrintMargin: boolean;
  onLoaded: Function;
  onChange: Function;
}
