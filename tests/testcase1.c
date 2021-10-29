void testcase1(int k)
{
	int x,a,y,b;
	//k = 2;
	if (k>10)
	{
	    a = k + 2;
	    x = k-1;
    }
    else
    {
        a = k * 2;
        x = k+1;
    }
     a=k;
    while (k!=x)
    {
        b = 2;
        x = a + k;
        y = a * b;
        k++;
    }
}
