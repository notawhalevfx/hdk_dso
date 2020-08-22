#pragma once

#include <FS/FS_Reader.h>
#include <FS/FS_Writer.h>
#include <FS/FS_Info.h>
#include <FS/FS_Utils.h>

namespace HDK_notawhale {

class FS_ProjectReaderHelper : public FS_ReaderHelper {
public:
	FS_ProjectReaderHelper();
    virtual ~FS_ProjectReaderHelper();

    virtual FS_ReaderStream	*createStream(const char *source, const UT_Options *options);

    virtual bool    splitIndexFileSectionPath(const char *source_section_path,
					UT_String &index_file_path, UT_String &section_name);

    virtual bool    combineIndexFileSectionPath(UT_String &source_section_path, 
                    const char *index_file_path, const char *section_name);
};

class FS_ProjectWriterHelper : public FS_WriterHelper {
public:
    FS_ProjectWriterHelper();
    virtual ~FS_ProjectWriterHelper();

    virtual FS_WriterStream *createStream(const char *source);
};

class FS_ProjectInfoHelper : public FS_InfoHelper {
public:
	     FS_ProjectInfoHelper();
    virtual ~FS_ProjectInfoHelper();

    virtual bool	 canHandle(const char *source);
    virtual bool	 hasAccess(const char *source, int mode);
    virtual bool	 getIsDirectory(const char *source);
    virtual int		 getModTime(const char *source);
    virtual int64 	 getSize(const char *source);
    virtual UT_String	 getExtension(const char *source);
    virtual bool	 getContents(const char *source,
				     UT_StringArray &contents,
				     UT_StringArray *dirs);
    virtual bool    getPathOnDisk(UT_String& path, 
                                  const char *file);
};

} // End HDK_notawhale