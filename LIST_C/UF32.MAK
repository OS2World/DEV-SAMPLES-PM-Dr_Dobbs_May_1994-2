Options =  /W3 /c /Gd- /Ge+ /Gm+ /Re /Kbegp /O+

$(a)uf.lib:	$(a)uf.obj
		if exist $(a)uf.lib del $(a)uf.lib
		lib $(a)uf.lib +$(a)uf.obj;
		if exist $(a).exe del $(a).exe

$(a)uf.obj:	$(a)uf.C $(a).H
		ICC.Exe $(Options) $(a)uf.C
