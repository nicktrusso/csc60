// Question: Explain the compiler's errors reported on line 8, 11, and 14
#include <stdio.h>
int main()
{
	int a, b;
	b = a = 7; /* a is an integer */
	const int *aPtr1 = &a; /* aPtr1 is a pointer to an integer of a */
        *aPtr1 = 9;
	//
	int *const aPtr2 = &a; /* aPtr2 is a pointer to an integer of a */
        aPtr2 = &b;
        //
        const int *const aPtr3 = &a;  /* aPtr2 is a pointer to an integer of a */
        *aPtr3 = 329;
        //
	return 0;
}

// Question: (1) Why your program is aborted? 
#include <stdio.h>
int main()
{
	int a;
 	int *aPtr;
        *aPtr = 5;
	printf("The address of a is %p"
		"\nThe value of aPtr is %p", &a, *aPtr);

	return 0;
}

// Questions: (1) Provide the hexadecimal values for foo_ptr in all assignment's statements involved foo_ptr  
//            (2) What are the decimal values of foo and bar before the return 0 statment? 
#include <stdio.h>
int main ()
{
    int foo = 42;
    int bar = -1;
    int *foo_ptr;

    foo_ptr = & foo;

    printf ("Get the existing values of foo, bar, foo_ptr, and * foo_ptr:\n");

    printf ("foo = %d\n", foo);
    printf ("bar = %d\n", bar);
    printf ("foo_ptr = %x\n", foo_ptr);
    printf ("* foo_ptr = %d\n", * foo_ptr);

    printf ("Change the value of * foo_ptr:\n");

    *foo_ptr = 99;

    printf ("foo = %d\n", foo);
    printf ("bar = %d\n", bar);
    printf ("foo_ptr = %x\n", foo_ptr);
    printf ("* foo_ptr = %d\n", * foo_ptr);

    printf ("Change the value of foo_ptr to & bar:\n");

    foo_ptr = &bar;

    printf ("foo = %d\n", foo);
    printf ("bar = %d\n", bar);
    printf ("foo_ptr = %x\n", foo_ptr);
    printf ("* foo_ptr = %d\n", * foo_ptr);

    return 0;
}

// Questions: (1) what is the hexadecimal of aptr right after line 16 is executed ?
//            (2) what is the value of debug command "p/x array_ptr - aptr" right after line 16 is executed?
//                Explain why you have this result ?
//            (3) what is the decimal value of *aptr after line 17 is executed ?
//            (4) what is the decimal value of *(array+2) 
#include <stdio.h>
int main ()
{
    int array[] = { 45, 67, 89 };
    int *array_ptr;
    int *aptr;
    array_ptr = array;
    printf(" first element: %i\n", *(array_ptr++));
    printf(" second element: %i\n", *(array_ptr++));
    printf(" third element: %i\n", *array_ptr);
    aptr = &array[0]; // line 16
	printf("QUESTION 1: h%x\n",aptr);
    aptr = array_ptr; // line 17
	printf("QUESTION 3: %d\n",*aptr);
	printf("QUESTION 4: %d\n",*(array+2));
    return 0;
}

// pfunction.c - to demo the function pointer technique
// Compile option: gcc pfunction.c -lm -g
// Questions: (1) where are the function pointers initialized ?
//            (2) how would you validate that the sin and cos functions are executed as advertised?  
//                See line 41.
#include <math.h>
#include <stdio.h>
void tabulate(double (*f)(double), double first, double last, double incr);
double mycos(double x);
double mysine(double x);
main()
{
		double final, increment, initial;
                double  (*fp)(double);
		printf ("Enter initial value: ");
		scanf ("%lf", &initial);
		printf ("Enter final value: ");
		scanf ("%lf", &final);
		printf ("Enter increment : ");
		scanf ("%lf", &increment);
                fp = mycos;
                printf("\nPointer function address for cos: %p\n",fp);
		printf("\n      x          cos(x) \n"
		         "  ----------  -----------\n");
		tabulate(fp, initial,final,increment);
                fp = mysine;
                printf("\nPointer function address for sin: %p\n",fp);
		printf("\n      x          sin (x) \n"
		"  ----------  -----------\n");
		tabulate(fp, initial,final,increment);
		return 0;
}
// when passed a pointer f prints a table showing the value of f
 void tabulate(double (*f) (double), double first, double last, double 		incr)
{
 	   double x;
           int i, num_intervals;
           num_intervals = ceil ( (last -first) /incr );
           for (i=0; i<=num_intervals; i++){
               x= first +i * incr;
               printf("%10.5f %10.5f\n", x , f(x)); // line 41
               // Can do this too: printf("%10.5f %10.5f\n", x , (*f) (x));
           }
}
double mycos(double x)
{
   return(cos(x));
}
double mysine(double x)
{
   return(sin(x));
}

// Filename:  struct.c
// Questions: (1) what is the value of *std_ptr at line 27
//            (2) what are the hexadecimal values of &std_ptr->ssid and &std_ptr->age at line 27
//            (3) what is the hexadecimal value of std_ptr at line 27 ? why this value is the same as &std_ptr->ssid ? 
//            (4) what are the debug command to print out the members (ssid, age, and name) of struct csus_student after line 29 ?
// 
#include <stdio.h>
#include <string.h>
struct csus_student {
     signed int ssid;	/* social security id */
     signed int age; 	/* name */ 
     char name[50];     /* name */  
};
typedef struct csus_student CsusStudent;

int main()
{
  CsusStudent studentone;
  struct csus_student student_one = { 1234,  18, "Student One" };
  printf ("id=%d age=%d name = %s\n", student_one.ssid, student_one.age, student_one.name);
  update_student(&student_one);
  printf ("id=%d age=%d name = %s\n", student_one.ssid, student_one.age, student_one.name);
  return 0; 
}
int update_student(struct csus_student *std_ptr) 
{
  std_ptr->ssid = 5678; // line 27
	printf("QUESTION 1: %d\n",*std_ptr);
	printf("QUESTION 3: %x\n",std_ptr);
  std_ptr->age = 19;
	printf("QUESTION 2: %x, %x\n",std_ptr->ssid,std_ptr->age);
  strcpy(std_ptr->name, "New student one"); // line 29
  return 0;
}

// Filename:  structval.c
// Questions: (1) what are the values for the CSUS struct's member before calling test_update_student 
//            (2) what are the values for the CSUS struct's member after calling test_update_student 
//            (3) explain the outcome ? 
// 
#include <stdio.h>
#include <string.h>
struct csus_student {
     signed int ssid;	/* social security id */
     signed int age; 	/* name */ 
     char name[50];     /* name */  
};
typedef struct csus_student CsusStudent;

int main()
{
  CsusStudent studentone;
  struct csus_student student_one = { 1234,  18, "Student One" };
  printf ("id=%d age=%d name = %s\n", student_one.ssid, student_one.age, student_one.name);
  test_update_student(student_one);
  printf ("id=%d age=%d name = %s\n", student_one.ssid, student_one.age, student_one.name);
  return 0; 
}
int test_update_student(struct csus_student temp_struct ) 
{
  temp_struct.ssid = 5678; // line 27
  temp_struct.age = 19;
  strcpy(temp_struct.name, "New student one"); // line 29
  return 0;
}

