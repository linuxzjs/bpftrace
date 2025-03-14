#include "ast/ast.h"

#include "gtest/gtest.h"

namespace bpftrace::test::ast {

using bpftrace::ast::AttachPoint;
using bpftrace::ast::AttachPointList;
using bpftrace::ast::Diagnostics;
using bpftrace::ast::Probe;

TEST(ast, probe_name_special)
{
  Diagnostics d;
  AttachPoint ap1(d, "", false, location());
  ap1.provider = "BEGIN";
  Probe begin(d, { &ap1 }, nullptr, {}, location());
  EXPECT_EQ(begin.name(), "BEGIN");

  AttachPoint ap2(d, "", false, location());
  ap2.provider = "END";
  Probe end(d, { &ap2 }, nullptr, {}, location());
  EXPECT_EQ(end.name(), "END");
}

TEST(ast, probe_name_kprobe)
{
  Diagnostics d;
  AttachPoint ap1(d, "", false, location());
  ap1.provider = "kprobe";
  ap1.func = "sys_read";

  AttachPoint ap2(d, "", false, location());
  ap2.provider = "kprobe";
  ap2.func = "sys_write";

  AttachPoint ap3(d, "", false, location());
  ap3.provider = "kprobe";
  ap3.func = "sys_read";
  ap3.func_offset = 10;

  Probe kprobe1(d, { &ap1 }, nullptr, {}, location());
  EXPECT_EQ(kprobe1.name(), "kprobe:sys_read");

  Probe kprobe2(d, { &ap1, &ap2 }, nullptr, {}, location());
  EXPECT_EQ(kprobe2.name(), "kprobe:sys_read,kprobe:sys_write");

  Probe kprobe3(d, { &ap1, &ap2, &ap3 }, nullptr, {}, location());
  EXPECT_EQ(kprobe3.name(),
            "kprobe:sys_read,kprobe:sys_write,kprobe:sys_read+10");
}

TEST(ast, probe_name_uprobe)
{
  Diagnostics d;
  AttachPoint ap1(d, "", false, location());
  ap1.provider = "uprobe";
  ap1.target = "/bin/sh";
  ap1.func = "readline";
  Probe uprobe1(d, { &ap1 }, nullptr, {}, location());
  EXPECT_EQ(uprobe1.name(), "uprobe:/bin/sh:readline");

  AttachPoint ap2(d, "", false, location());
  ap2.provider = "uprobe";
  ap2.target = "/bin/sh";
  ap2.func = "somefunc";
  Probe uprobe2(d, { &ap1, &ap2 }, nullptr, {}, location());
  EXPECT_EQ(uprobe2.name(), "uprobe:/bin/sh:readline,uprobe:/bin/sh:somefunc");

  AttachPoint ap3(d, "", false, location());
  ap3.provider = "uprobe";
  ap3.target = "/bin/sh";
  ap3.address = 1000;
  Probe uprobe3(d, { &ap1, &ap2, &ap3 }, nullptr, {}, location());
  EXPECT_EQ(
      uprobe3.name(),
      "uprobe:/bin/sh:readline,uprobe:/bin/sh:somefunc,uprobe:/bin/sh:1000");

  AttachPoint ap4(d, "", false, location());
  ap4.provider = "uprobe";
  ap4.target = "/bin/sh";
  ap4.func = "somefunc";
  ap4.func_offset = 10;
  Probe uprobe4(d, { &ap1, &ap2, &ap3, &ap4 }, nullptr, {}, location());
  EXPECT_EQ(uprobe4.name(),
            "uprobe:/bin/sh:readline,uprobe:/bin/sh:somefunc,uprobe:/bin/"
            "sh:1000,uprobe:/bin/sh:somefunc+10");

  AttachPoint ap5(d, "", false, location());
  ap5.provider = "uprobe";
  ap5.target = "/bin/sh";
  ap5.address = 10;
  Probe uprobe5(d, { &ap5 }, nullptr, {}, location());
  EXPECT_EQ(uprobe5.name(), "uprobe:/bin/sh:10");

  AttachPoint ap6(d, "", false, location());
  ap6.provider = "uretprobe";
  ap6.target = "/bin/sh";
  ap6.address = 10;
  Probe uprobe6(d, { &ap6 }, nullptr, {}, location());
  EXPECT_EQ(uprobe6.name(), "uretprobe:/bin/sh:10");
}

TEST(ast, probe_name_usdt)
{
  Diagnostics d;
  AttachPoint ap1(d, "", false, location());
  ap1.provider = "usdt";
  ap1.target = "/bin/sh";
  ap1.func = "probe1";
  Probe usdt1(d, { &ap1 }, nullptr, {}, location());
  EXPECT_EQ(usdt1.name(), "usdt:/bin/sh:probe1");

  AttachPoint ap2(d, "", false, location());
  ap2.provider = "usdt";
  ap2.target = "/bin/sh";
  ap2.func = "probe2";
  Probe usdt2(d, { &ap1, &ap2 }, nullptr, {}, location());
  EXPECT_EQ(usdt2.name(), "usdt:/bin/sh:probe1,usdt:/bin/sh:probe2");
}

TEST(ast, attach_point_name)
{
  Diagnostics d;
  AttachPoint ap1(d, "", false, location());
  ap1.provider = "kprobe";
  ap1.func = "sys_read";
  AttachPoint ap2(d, "", false, location());
  ap2.provider = "kprobe";
  ap2.func = "sys_thisone";
  AttachPoint ap3(d, "", false, location());
  ap3.provider = "uprobe";
  ap3.target = "/bin/sh";
  ap3.func = "readline";
  Probe kprobe(d, { &ap1, &ap2, &ap3 }, nullptr, {}, location());
  EXPECT_EQ(ap2.name(), "kprobe:sys_thisone");

  Probe uprobe(d, { &ap1, &ap2, &ap3 }, nullptr, {}, location());
  EXPECT_EQ(ap3.name(), "uprobe:/bin/sh:readline");
}

} // namespace bpftrace::test::ast
