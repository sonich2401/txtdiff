#pragma once

union settings{
	settings(){
		_stdout = false;
		_compare = false;
		_verify = false;
		_line_num = false;
		_help = false;
	}

	struct{
		bool _stdout;
		bool _compare;
		bool _verify;
		bool _line_num;
		bool _help;
	};

	long int n;
};