//
// ActiveMethodTest.cpp
//
// $Id: //poco/Main/Foundation/testsuite/src/ActiveMethodTest.cpp#13 $
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "ActiveMethodTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/ActiveMethod.h"
#include "Poco/Thread.h"
#include "Poco/Event.h"
#include "Poco/Exception.h"


using Poco::ActiveMethod;
using Poco::ActiveResult;
using Poco::Thread;
using Poco::Event;
using Poco::Exception;


namespace
{
	class ActiveObject
	{
	public:
		ActiveObject():
			testMethod(this, &ActiveObject::testMethodImpl),
			testVoid(this,&ActiveObject::testVoidOutImpl),
			testVoidInOut(this,&ActiveObject::testVoidInOutImpl),
			testVoidIn(this,&ActiveObject::testVoidInImpl)
		{
		}
		
		~ActiveObject()
		{
		}
		
		ActiveMethod<int, int, ActiveObject> testMethod;

		ActiveMethod<void, int, ActiveObject> testVoid;

		ActiveMethod<void, void, ActiveObject> testVoidInOut;

		ActiveMethod<int, void, ActiveObject> testVoidIn;
		
		void cont()
		{
			_continue.set();
		}
		
	protected:
		int testMethodImpl(const int& n)
		{
			if (n == 100) throw Exception("n == 100");
			_continue.wait();
			return n;
		}

		void testVoidOutImpl(const int& n)
		{
			if (n == 100) throw Exception("n == 100");
			_continue.wait();
		}

		void testVoidInOutImpl()
		{
			_continue.wait();
		}

		int testVoidInImpl()
		{
			_continue.wait();
			return 123;
		}
		
	private:
		Event _continue;
	};
}


ActiveMethodTest::ActiveMethodTest(const std::string& name): CppUnit::TestCase(name)
{
}


ActiveMethodTest::~ActiveMethodTest()
{
}


void ActiveMethodTest::testWait()
{
	ActiveObject activeObj;
	ActiveResult<int> result = activeObj.testMethod(123);
	assert (!result.available());
	activeObj.cont();
	result.wait();
	assert (result.available());
	assert (result.data() == 123);
	assert (!result.failed());
}


void ActiveMethodTest::testWaitInterval()
{
	ActiveObject activeObj;
	ActiveResult<int> result = activeObj.testMethod(123);
	assert (!result.available());
	try
	{
		result.wait(100);
		fail("wait must fail");
	}
	catch (Exception&)
	{
	}
	activeObj.cont();
	result.wait(10000);
	assert (result.available());
	assert (result.data() == 123);
	assert (!result.failed());
}


void ActiveMethodTest::testTryWait()
{
	ActiveObject activeObj;
	ActiveResult<int> result = activeObj.testMethod(123);
	assert (!result.available());
	assert (!result.tryWait(200));
	activeObj.cont();
	assert (result.tryWait(10000));
	assert (result.available());
	assert (result.data() == 123);
	assert (!result.failed());
}


void ActiveMethodTest::testFailure()
{
	ActiveObject activeObj;
	ActiveResult<int> result = activeObj.testMethod(100);
	result.wait();
	assert (result.available());
	assert (result.failed());
	std::string msg = result.error();
	assert (msg == "n == 100");
}


void ActiveMethodTest::testVoidOut()
{
	ActiveObject activeObj;
	ActiveResult<void> result = activeObj.testVoid(101);
	activeObj.cont();
	result.wait();
	assert (result.available());
	assert (!result.failed());
}


void ActiveMethodTest::testVoidInOut()
{
	ActiveObject activeObj;
	ActiveResult<void> result = activeObj.testVoidInOut();
	activeObj.cont();
	result.wait();
	assert (result.available());
	assert (!result.failed());
}


void ActiveMethodTest::testVoidIn()
{
	ActiveObject activeObj;
	ActiveResult<int> result = activeObj.testVoidIn();
	activeObj.cont();
	result.wait();
	assert (result.available());
	assert (!result.failed());
	assert (result.data() == 123);
}


void ActiveMethodTest::setUp()
{
}


void ActiveMethodTest::tearDown()
{
}


CppUnit::Test* ActiveMethodTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("ActiveMethodTest");

	CppUnit_addTest(pSuite, ActiveMethodTest, testWait);
	CppUnit_addTest(pSuite, ActiveMethodTest, testWaitInterval);
	CppUnit_addTest(pSuite, ActiveMethodTest, testTryWait);
	CppUnit_addTest(pSuite, ActiveMethodTest, testFailure);
	CppUnit_addTest(pSuite, ActiveMethodTest, testVoidOut);
	CppUnit_addTest(pSuite, ActiveMethodTest, testVoidIn);
	CppUnit_addTest(pSuite, ActiveMethodTest, testVoidInOut);

	return pSuite;
}
