#ifndef MUTEXUTILITY_H__
#define MUTEXUTILITY_H__

#include <Windows.h>

//Helper to specify Timeout
inline bool LOCK( HANDLE& aMutex, int aTimeOut )
{
	//Timeout
	if( WaitForSingleObject( aMutex, aTimeOut /*Do not change this variable*/ ) == WAIT_OBJECT_0 ) {
		return true;
	}
	return false;
}

//Helper to lock a mutex
inline bool LOCK( HANDLE& aMutex)
{
	//Fixed timeout of 0
	return LOCK( aMutex, 10 );
}

//Unlock mutex
inline bool UNLOCK( HANDLE& aMutex )
{
	return ReleaseSemaphore( aMutex ,1,NULL)?1:0;
}
#endif // MUTEXUTILITY_H__