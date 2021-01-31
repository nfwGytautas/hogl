#pragma once

// For the reference count
#include <atomic>

namespace hogl {
	/**
	 * Simple pointer wrapper that doesn't allow the deletion of the underlying pointer.
	 */
	template <typename T>
	class relay_ptr {
	public:
		/**
		 * Constructor with pointer
		 *
		 * @param object The pointer to be stored
		 */
		relay_ptr(T* aptr)
			: ptr(aptr)
		{ }

		/**
		 * Empty constructor
		 */
		relay_ptr()
			: ptr(nullptr)
		{ }

		/**
		 * Copy constructor
		 *
		 * @param other Another relay object
		 */
		relay_ptr(const relay_ptr<T>& relay)
		{
			ptr = relay.ptr;
		}

#ifdef _MEMORY_
		/**
		 * Create relay_ptr from unique_ptr
		 *
		 * @param aptr std::unique_ptr with T value
		 */
		relay_ptr(std::unique_ptr<T>& aptr)
			: ptr(aptr.get())
		{ }

		/**
		 * Create relay_ptr from shared_ptr
		 *
		 * @param aptr std::shared_ptr with T value
		 */
		relay_ptr(std::shared_ptr<T>& aptr)
			: ptr(aptr.get())
		{ }
#endif

		/**
		 * Destructor
		 */
		~relay_ptr() { }

		/**
		 * Set relay_ptr value to different ptr, the value of the original ptr is not changed
		 *
		 * @param other New value for this relay object
		 * @return This, but with new values
		 */
		relay_ptr<T>& operator=(const relay_ptr<T>& rhs) {
			ptr = rhs.ptr;
			return *this;
		}

		/**
		 * Set relay_ptr value to different ptr, the value of the original ptr is not changed
		 *
		 * @param other New value for this relay object
		 * @return This, but with new values
		 */
		relay_ptr<T>& operator=(T* rhs) {
			ptr = rhs;
			return *this;
		}

		/**
		 * Compares two relay pointers of type T by checking
		 * if the underlying pointers point to the same memory
		 *
		 * @param other The other relay pointer to compare to
		 * @return True if both relays point to the same memory
		 */
		bool operator==(const relay_ptr<T>& other) const {
			return ptr == other.ptr;
		}

		/**
		 * Checks if the underlying pointer is valid
		 *
		 * @return True if the pointer is not pointing to nullptr
		 */
		bool valid() const {
			return ptr != nullptr;
		}

		/**
		 * Member access operator
		 *
		 * @return Underlying object const pointer
		 */
		T* operator->() const {
			return ptr;
		}
	private:
		T* ptr;
	};

	/**
	 * A pointer wrapper similar to std::shared_ptr, in that it counts reference count,
	 * but it also provides functionality for polymorphism with as() function
	 */
	template <typename T>
	class ref {
	private:
		// ref count manipulation - increase by 1
		void increment() {
			(*m_ReferenceCount)++;
		}

		// ref count manipulation - decrease by 1
		int decrement() {
			return --(*m_ReferenceCount);
		}
	private:
		T* m_Object{ nullptr };
		std::atomic_int* m_ReferenceCount{ nullptr };

		template<class AsType>
		friend class ref;
	private:
		// Private constructor;
		ref(T* object, std::atomic_int* refcnt)
			: m_Object{ object }
			, m_ReferenceCount{ refcnt }
		{
			increment();
		}
	public:
		/**
		 * Empty constructor
		 */
		ref()
			: m_Object{ nullptr }
			, m_ReferenceCount{ nullptr }
		{
		}

		/**
		 * Constructor with pointer
		 *
		 * @param object The pointer to be stored
		 */
		ref(T* object)
			: m_Object{ object }
			, m_ReferenceCount{ new std::atomic_int(0) }
		{
			increment();
		}

		/**
		 * Destructor
		 */
		virtual ~ref() {
			if (m_ReferenceCount) {
				int decrementedCount = decrement();
				if (decrementedCount <= 0) {
					delete m_ReferenceCount;
					delete m_Object;
					m_ReferenceCount = nullptr;
					m_Object = nullptr;
				}
			}
		}

		/**
		 * Copy constructor
		 *
		 * @param other Another reference object
		 */
		ref(const ref<T>& other)
			: m_Object{ other.m_Object }
			, m_ReferenceCount{ other.m_ReferenceCount }
		{
			if (m_ReferenceCount) {
				increment();
			}
		}

		/**
		 * Assignment operator used to reassign the value of the reference object
		 *
		 * @param other New value for this reference object
		 * @return This, but with new values
		 */
		ref<T>& operator=(const ref<T>& other) {
			if (this != &other) {
				if (m_ReferenceCount) {
					if (decrement() == 0) {
						delete m_ReferenceCount;
						delete m_Object;
					}
				}
				m_Object = other.m_Object;
				m_ReferenceCount = other.m_ReferenceCount;
				increment();
			}
			return *this;
		}

		/**
		 * Compares two references of type T by checking 
		 * if the underlying pointers point to the same memory
		 *
		 * @param other The other reference to compare to
		 * @return True if both reference point to the same memory
		 */
		bool operator==(const ref<T>& other) const {
			return m_Object == other.m_Object;
		}

		/**
		 * Dereference operator used to get the object instance
		 *
		 * @return Underlying object c++ reference
		 */
		T& operator*() {
			return *m_Object;
		}

		/**
		 * Member access operator
		 *
		 * @return Underlying object const pointer
		 */
		T* operator->() const {
			return m_Object;
		}

		/**
		 * Creates a reference of specified type from current reference
		 * used in polymorphic types
		 *
		 * @tparam AsType The type of the new reference
		 * @return Reference with the specified AsType type
		 */
		template <typename AsType>
		ref<AsType> as() {
			return ref<AsType>(dynamic_cast<AsType*>(m_Object), m_ReferenceCount);
		}

		/**
		 * Checks if the underlying pointer is valid
		 *
		 * @return True if the pointer is not pointing to nullptr
		 */
		bool valid() const {
			return m_Object != nullptr;
		}

		
		/**
		 * Returns the underlying pointer as a relay_ptr which guarantees that the pointer won't be deleted
		 * but also there won't be any reference counting which means that if the reference is deleted the
		 * relay_ptr will be invalid
		 *
		 * @return Relay pointer created from the underlying pointer
		 */
		relay_ptr<T> relay() const {
			return relay_ptr<T>(m_Object);
		}

		/**
		 * Returns the underlying pointer as a relay_ptr which guarantees that the pointer won't be deleted
		 * but also there won't be any reference counting which means that if the reference is deleted the
		 * relay_ptr will be invalid
		 *
		 * @return Relay pointer created from the underlying pointer
		 */
		template<class AsType>
		relay_ptr<AsType> relay_as() const {
			return relay_ptr<AsType>(dynamic_cast<AsType*>(m_Object));
		}

		/**
		 * Current reference count of the object
		 *
		 * @return Number of references
		 */
		int ref_count() const {
			return *m_ReferenceCount;
		}
	};
}