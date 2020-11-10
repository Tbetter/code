/*
算法：
    二分查找-非递归
参数：
    arr   要查找的数组
    size  数组的大小
    key   要查找的值 
返回值：
    数组下标
*/
int BinarySearch(int *arr, int size, int key)
{
    if( arr == NULL || size == 0)
        return -1;
    
    int low = 0;
    int high = size - 1;
    int mid = 0;

    while(low <= high)
    {
        mid = (low + high)/2;
        
        if(arr[mid] < key)
            low = mid + 1;
        else if(arr[mid] > key)
            high = mid -1;
        else 
            return mid;
    }
}

/*
算法：
    二分查找-递归
参数：
    arr     要查找的数组
    low     数组的最小下标
    high    数组的最大小标    
    key     要查找的值 
返回值：
    数组下标
*/ 
int BinarySearchRecursive(int *arr, int low, int high, int key)
{
    if(low > high)
        return -1;

    int mid = (low + high)/2;

    if(arr[mid] < key)
        return BinarySearchRecursive(arr, mid + 1, high, key);
    else if(arr[mid] > key)
        return BinarySearchRecursive(arr, low, high - 1, key);
    else 
        return mid;
}
