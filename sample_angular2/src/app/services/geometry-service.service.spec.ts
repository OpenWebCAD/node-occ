/* tslint:disable:no-unused-variable */

import { TestBed, async, inject } from '@angular/core/testing';
import { GeometryServiceService } from './geometry-service.service';

describe('GeometryServiceService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [GeometryServiceService]
    });
  });

  it('should ...', inject([GeometryServiceService], (service: GeometryServiceService) => {
    expect(service).toBeTruthy();
  }));
});
