#pragma once

#include <functional>
#include "ChessCommon.h"
#if defined __WINDOWS__
#	include <Windows.h>
#elif defined __LINUX__
#	include <pthread.h>
#endif

namespace ChessEngine
{
	class CHESS_ENGINE_API Thread
	{
	public:

		Thread();
		virtual ~Thread();

		// Derivatives override this to perform work in the thread.
		virtual int ThreadFunc() = 0;

		// Start thread execution.
		bool SpawnThread();

		// The caller should signal the thread to shutdown before calling this.
		bool WaitForThreadExit(void);

		// It is bad practice to ever call this function if the thread is running.
		// If the thread is not running, it is just a no-op.
		void KillThread(void);

		bool IsStillRunning(void);

		static void Sleep(double timeoutMilliseconds);

	private:

#if defined __WINDOWS__
		static DWORD __stdcall ThreadMain(LPVOID param);
		HANDLE threadHandle;
#elif defined __LINUX__
		static void* ThreadMain(void* arg);
		pthread_t thread;
		volatile bool threadRunning;
#endif
	};

	class CHESS_ENGINE_API Semaphore
	{
	public:
		Semaphore(int32_t count)
		{
#if defined __WINDOWS__
			this->semaphoreHandle = ::CreateSemaphore(NULL, 0, count, NULL);
#elif defined __LINUX__
			// TODO: Write this.
#endif
		}

		virtual ~Semaphore()
		{
#if defined __WINDOWS__
			::CloseHandle(this->semaphoreHandle);
#endif
		}

		void Increment()
		{
#if defined __WINDOWS__
			::ReleaseSemaphore(this->semaphoreHandle, 1, NULL);
#endif
		}

		bool Decrement(double timeoutMilliseconds = -1.0)
		{
#if defined __WINDOWS__
			return WAIT_OBJECT_0 == ::WaitForSingleObject(this->semaphoreHandle, (timeoutMilliseconds >= 0.0f) ? (DWORD)timeoutMilliseconds : INFINITE);
#endif
		}

#if defined __WINDOWS__
		HANDLE semaphoreHandle;
#endif
	};

	class CHESS_ENGINE_API Mutex
	{
	public:
		Mutex()
		{
#if defined __WINDOWS__
			::InitializeCriticalSection(&this->criticalSection);
#elif defined __LINUX__
			pthread_mutex_init(&this->mutex, NULL);
#endif
		}

		virtual ~Mutex()
		{
#if defined __WINDOWS__
			::DeleteCriticalSection(&this->criticalSection);
#elif defined __LINUX__
			pthread_mutex_destroy(&this->mutex);
#endif
		}

#if defined __WINDOWS__
		CRITICAL_SECTION criticalSection;
#elif defined __LINUX__
		pthread_mutex_t mutex;
#endif
	};

	class CHESS_ENGINE_API MutexLocker
	{
	public:
		MutexLocker(Mutex& mutex)
		{
			this->cachedMutex = &mutex;
#if defined __WINDOWS__
			::EnterCriticalSection(&this->cachedMutex->criticalSection);
#elif defined __LINUX__
			pthread_mutex_lock(&this->cachedMutex->mutex);
#endif
		}

		virtual ~MutexLocker()
		{
#if defined __WINDOWS__
			::LeaveCriticalSection(&this->cachedMutex->criticalSection);
#elif defined __LINUX__
			pthread_mutex_unlock(&this->cachedMutex->mutex);
#endif
		}

	private:
		Mutex* cachedMutex;
	};

	class CHESS_ENGINE_API Event
	{
	public:
		Event()
		{
#if defined __WINDOWS__
			this->eventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
#elif defined __LINUX__
			// TODO: Write this.
#endif
		}

		virtual ~Event()
		{
#if defined __WINDOWS__
			if (this->eventHandle)
				CloseHandle(this->eventHandle);
#endif
		}

		void Signal()
		{
#if defined __WINDOWS__
			SetEvent(this->eventHandle);
#endif
		}

		void Wait()
		{
#if defined __WINDOWS__
			WaitForSingleObject(this->eventHandle, INFINITE);
#endif
		}

		static void WaitMultiple(const std::vector<Event*>& eventArray)
		{
			if (eventArray.size() == 0)
				return;
#if defined __WINDOWS__
			HANDLE* handleArray = new HANDLE[eventArray.size()];
			for (int i = 0; i < (signed)eventArray.size(); i++)
				handleArray[i] = eventArray[i]->eventHandle;
			WaitForMultipleObjects(eventArray.size(), handleArray, TRUE, INFINITE);
			delete[] handleArray;
#endif
		}

	private:
#if defined __WINDOWS__
		HANDLE eventHandle;
#endif
	};

	template<typename T>
	class CHESS_ENGINE_API LinkedList
	{
	public:

		LinkedList()
		{
			this->head = nullptr;
			this->tail = nullptr;
			this->count = 0;
		}

		virtual ~LinkedList()
		{
			this->RemoveAll();
		}

		class Node
		{
			friend class LinkedList;

		public:

			Node(T givenValue)
			{
				this->next = nullptr;
				this->prev = nullptr;
				this->value = givenValue;
				this->deleteFlag = nullptr;
			}

			virtual ~Node()
			{
				if (this->deleteFlag)
					*this->deleteFlag = true;
			}

			void Couple()
			{
				if (this->next)
					this->next->prev = this;

				if (this->prev)
					this->prev->next = this;
			}

			void Decouple()
			{
				if (this->next)
					this->next->prev = this->prev;

				if (this->prev)
					this->prev->next = this->next;
			}

			T value;

			Node* GetNext() { return this->next; }
			Node* GetPrev() { return this->prev; }

			const Node* GetNext() const { return this->next; }
			const Node* GetPrev() const { return this->prev; }

			bool* deleteFlag;

		private:
			Node* next;
			Node* prev;
		};

		void AddTail(T value)
		{
			this->InsertAfter(this->tail, value);
		}

		void AddHead(T value)
		{
			this->InsertBefore(this->head, value);
		}

		void InsertAfter(Node* after, T value)
		{
			Node* node = new Node(value);

			if (!after)
				this->head = this->tail = node;
			else
			{
				node->prev = after;
				node->next = after->next;
				node->Couple();
				if (!node->next)
					this->tail = node;
			}

			this->count++;
		}

		void InsertBefore(Node* before, T value)
		{
			Node* node = new Node(value);

			if (!before)
				this->head = this->tail = node;
			else
			{
				node->next = before;
				node->prev = before->prev;
				node->Couple();
				if (!node->prev)
					this->head = node;
			}

			this->count++;
		}

		void Remove(Node* node)
		{
			if (this->head == node)
				this->head = this->head->next;

			if (this->tail == node)
				this->tail = this->tail->prev;

			node->Decouple();
			delete node;
			this->count--;
		}

		void RemoveAll()
		{
			while (this->count > 0)
				this->Remove(this->head);
		}

		T Find(std::function<bool(T)> predicate, T notFoundValue, bool removeIfFound = false)
		{
			T returnValue = notFoundValue;

			for (Node* node = this->head; node; node = node->next)
			{
				if (predicate(node->value))
				{
					returnValue = node->value;

					if (removeIfFound)
						this->Remove(node);

					break;
				}
			}

			return returnValue;
		}

		Node* GetHead() { return this->head; }
		Node* GetTail() { return this->tail; }

		const Node* GetHead() const { return this->head; }
		const Node* GetTail() const { return this->tail; }

		unsigned int GetCount() const { return this->count; }

	private:

		Node* head;
		Node* tail;
		unsigned int count;
	};

	template<typename T>
	CHESS_ENGINE_API void DeleteList(LinkedList<T>& list)
	{
		while (list.GetCount() > 0)
		{
			auto node = list.GetHead();
			T value = node->value;
			delete value;
			list.Remove(node);
		}
	}

	// Note that an important principle of any mutex locking is that it be
	// done as tightly as possible.  Here, more often than not, the mutex
	// is locked only long enough to perform some operation that should
	// be as fast as possible.
	template<typename T>
	class CHESS_ENGINE_API ThreadSafeList
	{
	public:

		ThreadSafeList()
		{
		}

		virtual ~ThreadSafeList()
		{
		}

		uint32_t GetCount() const
		{
			return this->linkedList.GetCount();
		}

		void AddTail(T value)
		{
			MutexLocker locker(this->mutex);
			this->linkedList.AddTail(value);
		}

		void AddHead(T value)
		{
			MutexLocker locker(this->mutex);
			this->linkedList.AddHead(value);
		}

		bool RemoveTail(T& value)
		{
			bool removed = false;
			if (this->linkedList.GetCount() > 0)	// Avoid mutex luck if unnecessary.
			{
				MutexLocker locker(this->mutex);
				if (this->linkedList.GetCount() > 0)	// Avoid race condition.
				{
					value = this->linkedList.GetTail()->value;
					this->linkedList.Remove(this->linkedList.GetTail());
					removed = true;
				}
			}
			return removed;
		}

		bool RemoveHead(T& value)
		{
			bool removed = false;
			if (this->linkedList.GetCount() > 0)	// Avoid mutex luck if unnecessary.
			{
				MutexLocker locker(this->mutex);
				if (this->linkedList.GetCount() > 0)	// Avoid race condition.
				{
					value = this->linkedList.GetHead()->value;
					this->linkedList.Remove(this->linkedList.GetHead());
					removed = true;
				}
			}
			return removed;
		}

		void Delete()
		{
			MutexLocker locker(this->mutex);
			DeleteList<T>(this->linkedList);
		}

		T Find(std::function<bool(T)> predicate, T notFoundValue, bool removeIfFound = false)
		{
			MutexLocker locker(this->mutex);
			return this->linkedList.Find(predicate, notFoundValue, removeIfFound);
		}

	private:

		Mutex mutex;
		LinkedList<T> linkedList;
	};
}