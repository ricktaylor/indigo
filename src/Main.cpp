///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2013 Rick Taylor
//
// This file is part of the Indigo boardgame engine.
//
// Indigo is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Indigo is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Indigo.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////

#include "App.h"
#include "Render.h"

#include <stdlib.h>
#include <limits.h>

#if defined(_WIN32)
#include <shlwapi.h>
#endif

#if defined(HAVE_UNISTD_H)
#include <signal.h>
#endif

#include <OOBase/BTree.h>
#include <OOBase/Morton.h>

static bool s_is_debug = false;

bool Indigo::is_debug()
{
	return s_is_debug;
}

static int help()
{
	OOBase::stdout_write("Indigo - The boardgame engine.\n\n"
		"Please consult the documentation at https://github.com/ricktaylor/indigo for further information.\n\n"
		"Usage: indigo [options]\n\n"
		"Options:\n"
		"  --help (-h)      Display this help text\n"
		"\n"
		"  --config-file (-f) <file_path>  Use the specified configuration file\n"
		"\n");

	return EXIT_SUCCESS;
}

static bool critical_failure(const char* msg)
{
	OOBase::stderr_write(msg);
	OOBase::stderr_write("\n");
	return true;
}

static int exit_failure(const char* fmt, ...)
{
	va_list args;
	va_start(args,fmt);

	OOBase::ScopedString msg;
	int err = msg.vprintf(fmt,args);

	va_end(args);

	if (err == 0)
		OOBase::stderr_write(msg.c_str());

	return EXIT_FAILURE;
}

static bool load_config(OOBase::CmdArgs::options_t& options)
{
	int err = 0;

#if defined(_WIN32)
	// Read from WIN32 registry
	err = OOBase::ConfigFile::load_registry(HKEY_LOCAL_MACHINE,"Software\\Indigo",config_args);
	if (err && err != ERROR_FILE_NOT_FOUND)
		LOG_ERROR_RETURN(("Failed read system registry: %s",OOBase::system_error_text(err)),false);
#endif

	// Determine conf file
	OOBase::String strFile;
	if (!options.find("config-file",strFile))
	{
#if defined(HAVE_UNISTD_H)
		if (access(".indigo.conf",F_OK) == 0)
		{
			if (!strFile.assign(".indigo.conf"))
				LOG_ERROR_RETURN(("Failed assign string"),false);
		}
		else
		{
			// Get user directory and check for .indigo.conf there
			OOBase::POSIX::pw_info info(getuid());
			if (!(!info) && info->pw_dir)
			{
				if (!strFile.assign(info->pw_dir) || !strFile.append("/.indigo.conf"))
					LOG_ERROR_RETURN(("Failed assign string"),false);

				if (access(strFile.c_str(),F_OK) != 0)
					strFile.clear();
			}

			if (strFile.empty())
			{
				if (access("/etc/indigo.conf",F_OK) == 0)
					if (!strFile.assign("/etc/indigo.conf"))
						LOG_ERROR_RETURN(("Failed assign string"),false);
			}
		}
#endif
	}

	// Load from config file
	if (!strFile.empty())
	{
		const char* rpath = strFile.c_str();

#if defined(HAVE_REALPATH)
		OOBase::SharedPtr<char> rp = OOBase::make_shared<char,OOBase::CrtAllocator>(realpath(strFile.c_str(),NULL));
		if (rp)
			rpath = rp.get();
#endif
		OOBase::Logger::log(OOBase::Logger::Information,"Using configuration file: '%s'",rpath);

		OOBase::ConfigFile::error_pos_t error = {0};
		err = OOBase::ConfigFile::load(strFile.c_str(),options,&error);
		if (err == EINVAL)
			LOG_ERROR_RETURN(("Failed read configuration file %s: Syntax error at line %lu, column %lu",rpath,(unsigned long)error.line,(unsigned long)error.col),false);
		else if (err != ENOENT)
			LOG_ERROR_RETURN(("Failed load configuration file %s: %s",rpath,OOBase::system_error_text(err)),false);

		OOBase::Logger::log(OOBase::Logger::Information,"Configuration loaded successfully");
	}
	else
	{
		OOBase::Logger::log(OOBase::Logger::Information,"No configuration file in use");
	}

	return true;
}

#if defined(_WIN32)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, const char* argv[])
#endif
{
	// Set critical failure handler
	OOBase::SetCriticalFailure(&critical_failure);

	// Get the debug ENV variable
	{
		OOBase::String str;
		OOBase::Environment::getenv("INDIGO_DEBUG",str);
		s_is_debug = (str == "true");
	}

	// Set up the command line arguments
	OOBase::CmdArgs cmd_args;
	cmd_args.add_option("help",'h');
	cmd_args.add_option("config-file",'f',true);
	cmd_args.add_option("debug");
#if defined(_WIN32)
	cmd_args.add_option("console");
#endif

	// Parse command line
	OOBase::CmdArgs::options_t options;
	OOBase::CmdArgs::arguments_t args;
#if defined(_WIN32)
	int err = cmd_args.parse(options,args);
#else
	int err = cmd_args.parse(argc,argv,options,args);
#endif
	if (err	!= 0)
	{
		OOBase::String strErr;
		if (options.find("missing",strErr))
			return exit_failure("Missing value for option %s\n",strErr.c_str());
		else if (options.find("unknown",strErr))
			return exit_failure("Unknown option %s\n",strErr.c_str());
		else
			return exit_failure("Failed to parse command line: %s\n",OOBase::system_error_text(err));
	}

	if (options.exists("debug"))
		s_is_debug = true;

	if (options.exists("help"))
		return help();

	// Start the logger
	OOBase::Logger::set_source_file(__FILE__);
#if defined(_WIN32)
	if (s_is_debug)
	{
		OOBase::Logger::connect_debug_log();
		if (args.exists("console") && AllocConsole())
		{
			OOBase::Logger::connect_stderr_log();
			OOBase::Logger::connect_stdout_log();
		}
	}
#else
	OOBase::Logger::connect_stderr_log();
	OOBase::Logger::connect_stdout_log();
#endif

#if defined(HAVE_UNISTD_H)
	// Ignore SIGCHLD and SIGPIPE
	sigset_t sigset;
	err = sigemptyset(&sigset);
	if (!err)
		err = sigaddset(&sigset, SIGCHLD);
	if (!err)
		err = sigaddset(&sigset, SIGPIPE);
	if (err)
		err = errno;
	else
		err = pthread_sigmask(SIG_BLOCK, &sigset, NULL);
	if (err)
		LOG_ERROR_RETURN(("Failed to adjust signals: %s",OOBase::system_error_text(err)),EXIT_FAILURE);
#endif

	// Load up our global configuration arguments
	if (!load_config(options))
		return EXIT_FAILURE;

	// Start our two main threads
	return Indigo::start_render_thread(&Indigo::Application::run,options,args) ? EXIT_SUCCESS : EXIT_FAILURE;
}
