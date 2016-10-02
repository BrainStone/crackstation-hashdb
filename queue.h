#ifndef CRACKSTATION_QUEUE_H
#define CRACKSTATION_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

template <typename T>
class Queue {
public:
	Queue() : size_( 0 ), limit_( 0 ) {}
	Queue( size_t limit ) : size_( 0 ), limit_( limit ) {}

	T pop() {
		std::unique_lock<std::mutex> mlock( mutex_ );

		while ( size_ == 0 ) {
			cond_.wait( mlock );
		}

		auto item = queue_.front();
		queue_.pop();
		--size_;

		unlockIfUnderLimit();

		return item;
	}

	void pop( T& item ) {
		std::unique_lock<std::mutex> mlock( mutex_ );

		while ( size_ == 0 ) {
			cond_.wait( mlock );
		}

		item = queue_.front();
		queue_.pop();
		--size_;

		unlockIfUnderLimit();
	}

	void push( const T& item ) {
		lockIfOverLimit();

		std::unique_lock<std::mutex> mlock( mutex_ );

		queue_.push( item );
		++size_;

		mlock.unlock();
		cond_.notify_one();
	}

	template<typename T_Sub>
	void push( const T_Sub& item ) {
		push( (const T&)item );
	}

	void push( T&& item ) {
		lockIfOverLimit();

		std::unique_lock<std::mutex> mlock( mutex_ );

		queue_.push( std::move( item ) );
		++size_;

		mlock.unlock();
		cond_.notify_one();
	}

	template<typename T_Sub>
	void push( T_Sub&& item ) {
		push( (T&&)item );
	}

	int size() {
		return size_;
	}

private:
	std::queue<T> queue_;
	std::mutex mutex_;
	std::mutex capMutex_;
	std::condition_variable cond_;
	std::condition_variable capCond_;
	std::atomic<size_t> size_;
	const size_t limit_;

	void lockIfOverLimit() {
		if ( limit_ == 0 )
			return;

		std::unique_lock<std::mutex> capLock( capMutex_ );

		if ( size_ >= limit_ ) {
			capCond_.wait( capLock );
		}
	}

	void unlockIfUnderLimit() {
		if ( limit_ == 0 )
			return;

		if ( size_ == (limit_ / 2) )
			capCond_.notify_all();
	}
};

#endif
