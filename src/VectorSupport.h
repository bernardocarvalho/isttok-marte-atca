
#if !defined VECTORSUPPORT_H
#define VECTORSUPPORT_H


//	Selection Sort, very basic sort, for large arrays use another algorithm, also duplication of memory, not recomented for using with very large data sets, O(N²)
bool DualVectorSort(int vector_dim, double * vector_x, double * vector_y){
	
	if(vector_dim <1){
		CStaticAssertErrorCondition(InitialisationError,"SVGGraphicSupport.h !!! DualVectorSort vector_dim lower than 1, vector_dim =%d",vector_dim);
		return False;
	} 
	if(vector_dim == 1) return True;
	
	int i;
	int alfa;
	double temp;
	double min = *vector_x;
	int pos;
	
	for(alfa=0;alfa<vector_dim-1;alfa++){	
		pos=alfa;
		min=*(vector_x+alfa);
		
		for (i=alfa;i < vector_dim;i++){
			if (*(vector_x+i) <= min){
				pos = i;
				min = *(vector_x+i);
			}	
		}	

		temp = *(vector_x+alfa);
		*(vector_x+alfa) = *(vector_x+pos);
		*(vector_x+pos) = temp;
		temp = *(vector_y+alfa);
		*(vector_y+alfa) = *(vector_y+pos);
		*(vector_y+pos) = temp;
	}
	
	return True;
}

//	use only after the vectors are sorted
bool RemoveRepeatedValues(int * vector_dim, double * vector_x, double * vector_y){
	
	if(*vector_dim <1){
		CStaticAssertErrorCondition(InitialisationError,"SVGGraphicSupport.h !!! RemoveRepeatedValues vector_dim lower than 1, vector_dim =%d",*vector_dim);
		return False;
	} 
	if(*vector_dim == 1) return True;
	
	int i=0;
	int alfa;
	
	for (alfa = 0; alfa < *vector_dim-1; alfa++){

		if ( *(vector_x+alfa) == *(vector_x+alfa+1)){
			
			for (i=alfa; i < *vector_dim-2; i++){
				*(vector_x+i+1) = *(vector_x+i+2);
				*(vector_y+i+1) = *(vector_y+i+2);
			}
			alfa--;
			*vector_dim -=1;
		}
	}

	return True;
}


#endif
