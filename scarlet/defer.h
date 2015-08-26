#ifndef _SCARLET_DEFER_H
#define _SCARLET_DEFER_H
#include <functional>
#include <stack>

namespace scarlet{
		class defer
		{
		public:

			~defer(){
				while (!m_func_stack.empty()){
					m_func_stack.top()();
					m_func_stack.pop();
				}
			}

			void push(std::function<void()> _func_ptr){
				m_func_stack.push(_func_ptr);
			}
		protected:
			std::stack < std::function<void()>> m_func_stack;
		};
}
#endif
