## 解决1+不开机（8E）
```c
__int64 init_module()
{
  int hmbird_version_type; // w8
  __int64 node_opts_by_path; // x0
  const char *s1_1; // x19
  char *s1[2]; // [xsp+0h] [xbp-10h] BYREF

  s1[1] = *(char **)(_ReadStatusReg(SP_EL0) + 1568);
  hmbird_version_type = ::hmbird_version_type;
  s1[0] = 0;
  if ( !::hmbird_version_type )
  {
    node_opts_by_path = of_find_node_opts_by_path("/soc/oplus,hmbird/version_type", 0);
    if ( node_opts_by_path && (of_property_read_string(node_opts_by_path, "type", s1), (s1_1 = s1[0]) != 0) )
    {
      if ( !strncmp(s1[0], "HMBIRD_OGKI", 0xBu) )
      {
        hmbird_version_type = 2;
      }
      else if ( !strncmp(s1_1, "HMBIRD_GKI", 0xAu) )
      {
        hmbird_version_type = 1;
      }
      else
      {
        hmbird_version_type = 3;
      }
    }
    else
    {
      hmbird_version_type = 3;
    }
    ::hmbird_version_type = hmbird_version_type;
  }
  if ( hmbird_version_type == 1 )
    scx_init();
  _ReadStatusReg(SP_EL0);
  return 0;
}
```
