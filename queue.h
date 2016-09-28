#ifndef CRACKSTATION_QUEUE_H
#define CRACKSTATION_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

template <typename T>
class Queue {
public:
	T() = default;
	T( size_t limit ) : limit_( limit ) {}

	T pop() {
		std::unique_lock<std::mutex> mlock( mutex_ );

		while ( queue_.empty() ) {
			cond_.wait( mlock );
		}

		unlockIfUnderLimit();

		auto item = queue_.front();
		queue_.pop();

		return item;
	}

	void pop( T& item ) {
		std::unique_lock<std::mutex> mlock( mutex_ );

		while ( queue_.empty() ) {
			cond_.wait( mlock );
		}

		unlockIfUnderLimit();

		item = queue_.front();
		queue_.pop();
	}

	void push( const T& item ) {
		std::unique_lock<std::mutex> mlock( mutex_ );

		lockIfOverLimit();

		queue_.push( item );

		mlock.unlock();
		cond_.notify_one();
	}

	template<typename T_Sub>
	void push( const T_Sub& item ) {
		push( (const T&)item );
	}

	void push( T&& item ) {
		std::unique_lock<std::mutex> mlock( mutex_ );

		lockIfOverLimit();

		queue_.push( std::move( item ) );

		mlock.unlock();
		cond_.notify_one();
	}

	template<typename T_Sub>
	void push( T_Sub&& item ) {
		push( (T&&)item );
	}

	int size() {
		std::unique_lock<std::mutex> mlock( mutex_ );

		return queue_.size();
	}

private:
	std::queue<T> queue_;
	std::mutex mutex_;
	std::mutex capMutex_;
	std::condition_variable cond_;
	const size_t limit_;

	void lockIfOverLimit() {
		if ( limit_ == 0 )
			return;

		if ( queue_.size() >= limit_ ) {
			capMutex_.lock();
			capMutex_.lock();
			capMutex_.unlock();
		}
	}

	void unlockIfUnderLimit() {
		if ( limit_ == 0 )
			return;

		if ( queue_.size() == (limit_ / 2) )
			capMutex_.try_lock();

		capMutex_.unlock();
	}
};

#endif
