void disctrl(char *line);
void unctrl(char *line);
void ucTrim(char *line);
void ucTrimEdge(char *line);
void ucTrimSqLead(char *line);
void ucTrimSqTrail(char *line);
autoList_t *ucStrTokenize(char *line);
autoList_t *ucStrTokenizeDs(char *line, char *delims);
void removeBlank(char *line);

void ucTrimAllLine(autoList_t *lines);
void ucTrimEdgeAllLine(autoList_t *lines);
void ucTrimSqLeadAllLine(autoList_t *lines);
void ucTrimSqTrailAllLine(autoList_t *lines);

autoList_t *readResourceLines(char *file);
autoList_t *readResourcePaths(char *pathListFile);
autoList_t *readResourceFilesLines(char *fileListFile);
autoList_t *readResourceLinesList(char *file);
autoList_t *discerpHeaders(autoList_t *linesList);
char *readResourceText(char *file);
