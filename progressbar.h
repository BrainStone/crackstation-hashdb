#ifndef CRACKSTATION_PROGRESSBAR_H
#define CRACKSTATION_PROGRESSBAR_H

class ProgressBar {
public:
	ProgressBar();
	~ProgressBar();

	void init();
	void finish();

	void updateProgress();
};

#endif
