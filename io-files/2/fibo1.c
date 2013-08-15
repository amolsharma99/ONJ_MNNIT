#include<stdio.h>
int f(int n)
{
	if(n==0) return 0;s
	if(n<=2)	return 1;
	else return f(n-1) + f(n-2);
}
int main()
{
	int n,t;
	scanf("%d",&t);
	while(t--)
	{
		scanf("%d",&n);
		printf("%d\n",f(n));
	}
	return 0;
}
