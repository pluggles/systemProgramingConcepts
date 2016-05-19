// A test of boundary constraints for the malloc() assignment
// The theory is that we need to keep our blocks of memory on 
// 'good' i.e. boundary aligned address so that our addresses
// are properly aligned ? 
#include <unistd.h>
#include <string.h>
#include <stdio.h>
int main()
{  void * P;
     typedef 
          struct {
               int Field1;
               void * Ptr; // to another Thing ? 
               } Thing;
Thing AThing;Thing *AThingPtr;

printf("current heap break %x \n",sbrk(0));

AThingPtr = (Thing *) (sbrk(1000));
AThingPtr =  AThingPtr  + 1;
printf("current Thing Pointer Location  %x \n",AThingPtr);
int AThingAddress = (int) (AThingPtr);
AThingAddress = AThingAddress + 1;
AThingPtr = (void *) AThingAddress;
printf("current Thing Pointer Location  %x \n",AThingPtr);

  memcpy( (void *) (AThingPtr),&AThing,sizeof(AThing));
     AThingPtr->Field1 = 0xFACECAFE;AThingPtr->Field1++;
     AThingPtr->Ptr = (void *) AThingPtr + sizeof(Thing);
          // Pointing to next block of memory ? 
          // Gawd, I hope this seg faults 
}
