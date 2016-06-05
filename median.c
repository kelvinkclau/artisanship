int median(List l) {
   int median = 0;
   if (l->head != NULL){
      Node curr1 = l->head;
      Node curr2 = l->head;
      
      while(curr1->next != NULL && curr1->next->next != NULL){
         curr1 = fast->next->next;
         curr2 = curr2->next;
      }
      if (curr1->next == NULL){
         median = curr2->value;
      } else {
         median = (curr2->value + curr2->next->value) / 2;
      }
   }
   return median;   
}
 