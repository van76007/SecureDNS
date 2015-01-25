/**
 * @file
 * @author snu <snu@csis.mu>
 *
 * @brief This file defines exceptions.
 *
 */

#ifndef NAMESERVER_EXCEPTIONS_H
#define NAMESERVER_EXCEPTIONS_H


#include <string>

namespace Nameserver {

	/**
	 * @brief InitException. Exception during initialization
	 *
	 * This exception is thrown during initialization phase
	 *
	 */
	class MainException {

	public:
       /**
        * @brief Init Exception constructor
	* @param whatMessage Message supplied by what method
        * @param userMsg The message to the UI
        */
		MainException(const std::string& whatMessage) :

			mWhatMessage(whatMessage)
		{};
	   
		virtual ~MainException() throw() {};
		virtual const char* what() const throw() {return mWhatMessage.c_str(); };
//		std::string GetUIMessage() const {return mUImessage; };
		
	protected:
		std::string mWhatMessage, mUImessage;
	private:

	}; //InitException

	class RunException {

	public:
       /**
        * @brief Runtime Exception constructor
		* @param whatMessage Message supplied by what method
        * @param userMsg The message to the UI
        */
		RunException(const std::string& whatMessage) :
			mWhatMessage(whatMessage) 
		{};
	   
		virtual ~RunException() throw() {};
		virtual const char* what() const throw() {return mWhatMessage.c_str(); };
//		std::string GetUIMessage() const {return mUImessage; };
		
	protected:
		std::string mWhatMessage, mUImessage;
	private:

	}; //RuntimeException
}; //namespace Nameserver


#endif //NAMESERVER_EXCEPTIONS_H
