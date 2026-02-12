This vcxproj show an erroneous way of assigning custom .manifest file.

On VC2010, the GuiRc.manifest will not be effective.

On VC2015, the linker asserts error:

> CVTRES : fatal error CVT1100: duplicate resource.  type:MANIFEST, name:1, language:0x0409

The solution is: Add linker option `/manifest:NO`, so that VC linker respects our GuiRc.manifest .

The corresponding MSBuild tag is:

```
	<PropertyGroup>
		<GenerateManifest>false</GenerateManifest>
	</PropertyGroup>
```

See my Evernote (20251112.m1) for detail.
