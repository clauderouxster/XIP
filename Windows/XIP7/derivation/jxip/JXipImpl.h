/*
	Copyright (c) 1999,2004 Xerox Corporation,
	project XIP (Xerox Incremental Parser)
	Xerox Research Centre Europe
	All Rights Reserved

	Copyright protection claimed includes all forms and matters of copyrightable
	material and information now allowed by statutory or judicial law or
	hereinafter granted, including without limitation, material generated from
	the software programs which are displayed on the screen such as icons,
	screen display looks, etc. All rights reserved. No part of this document
	may be reproduced, stored in a retrieval system, or transmitted, in any
	form or by any means, electronic, mechanical, photocopied, recorded, or
	otherwise, without prior written consent from XEROX corporation.

*/

string jstringToString(JNIEnv* env, jstring str);
jstring jstringFromChars(JNIEnv* env, const char *chars);
jstring jstringFromString(JNIEnv* env, string str);
