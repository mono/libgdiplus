#!/usr/bin/perl
#
# Copyright © 2005 Mozilla Corporation
#
# Permission to use, copy, modify, distribute, and sell this software
# and its documentation for any purpose is hereby granted without
# fee, provided that the above copyright notice appear in all copies
# and that both that copyright notice and this permission notice
# appear in supporting documentation, and that the name of
# Mozilla Corporation not be used in advertising or publicity pertaining to
# distribution of the software without specific, written prior
# permission. Mozilla Corporation makes no representations about the
# suitability of this software for any purpose.  It is provided "as
# is" without express or implied warranty.
#
# MOZILLA CORPORTAION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
# SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS, IN NO EVENT SHALL MOZILLA CORPORATION BE LIABLE FOR ANY SPECIAL,
# INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
# OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
# IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# Author: Vladimir Vukicevic <vladimir@pobox.com>
#

##
## Takes all the *.log files in the current directory and spits out
## html to stdout that can be used to view all the test results at once.
##

# show reference images
my $config_show_ref = 0;

# show fail images
my $config_show_fail = 1;

# show all results, even passes
my $config_show_all = 0;

# end of config options

my $tests = {};

my $teststats = {};

foreach (<*.log>) {
  (open LOG, "$_") || next;
  while (<LOG>) {
    next unless /^TEST: (.*) TARGET: (.*) FORMAT: (.*) OFFSET: (.*) RESULT: (.*)$/;
    $tests->{$1} = {} unless $tests->{$1};
    $tests->{$1}->{$2} = {} unless $tests->{$1}->{$2};
    $tests->{$1}->{$2}->{$3} = {} unless $tests->{$1}->{$2}->{$3};
    $tests->{$1}->{$2}->{$3}->{$4} = $5;

    $teststats->{$2} = {"PASS" => 0, "FAIL" => 0, "XFAIL" => 0, "UNTESTED" => 0}
      unless $teststats->{$2};
    ($teststats->{$2}->{$5})++;
  }
  close LOG;
}

my $targeth = {};
my $formath = {};
my $offseth = {};

foreach my $testname (sort(keys %$tests)) {
  my $v0 = $tests->{$testname};
  foreach my $targetname (sort(keys %$v0)) {
    my $v1 = $v0->{$targetname};

    $targeth->{$targetname} = 1;
    foreach my $formatname (sort(keys %$v1)) {
      my $v2 = $v1->{$formatname};

      $formath->{$formatname} = 1;
      foreach my $offsetval (sort(keys %$v2)) {
        $offseth->{$offsetval} = 1;
      }
    }
  }
}

my @targets = sort(keys %$targeth);
my @formats = sort(keys %$formath);
my @offsets = sort(keys %$offseth);

sub printl {
  print @_, "\n";
}

printl '<html><head>';
printl '<title>Cairo Test Results</title>';
printl '<style type="text/css">';
printl 'a img { border: none; }';
printl '.PASS { background-color: #009900; min-width: 1em; }';
printl '.FAIL { background-color: #990000; }';
printl '.XFAIL { background-color: #999900; }';
printl '.UNTESTED { background-color: #333333; }';
printl '.PASSstr { color: #009900; }';
printl '.FAILstr { color: #990000; }';
printl '.XFAILstr { color: #999900; }';
printl '.UNTESTEDstr { color: #333333; }';
printl 'img { max-width: 15em; min-width: 3em; }';
printl 'td { vertical-align: top; }';
printl '</style>';
printl '<body>';

printl '<table border="1">';
print '<tr><th>Test</th>';
print '<th>Ref</th>' if $config_show_ref;

foreach my $target (@targets) {
  print '<th>', $target, '</th>';
}
printl '</tr>';

print '<tr><td></td>';
print '<td></td>' if $config_show_ref;

foreach my $target (@targets) {
  print '<td>';
  print '<span class="PASSstr">', $teststats->{$target}->{"PASS"}, '</span>/';
  print '<span class="FAILstr">', $teststats->{$target}->{"FAIL"}, '</span>/';
  print '<span class="XFAILstr">', $teststats->{$target}->{"XFAIL"}, '</span>/';
  print '<span class="UNTESTEDstr">', $teststats->{$target}->{"UNTESTED"}; '</span>';
  print '</td>';
}
printl '</tr>';

sub testref {
  my ($test, $format, $rest) = @_;
  my $fmtstr = "";
  if ($format eq "rgb24") {
    $fmtstr = "-rgb24";
  }

  return "$test$fmtstr-ref.png";
}

sub testfiles {
  my ($test, $target, $format, $offset, $rest) = @_;
  my $fmtstr = "";
  my $offstr = "";
  if ($format eq "rgb24") {
    $fmtstr = "-rgb24";
  } elsif ($format eq "argb32") {
    $fmtstr = "-argb32";
  }
  if ($offset ne "0") {
    $offstr = "-" . $offset;
  }

  return ("out" => "$test-$target$fmtstr$offstr-out.png",
	  "diff" => "$test-$target$fmtstr$offstr-diff.png");
}

foreach my $test (sort(keys %$tests)) {
  foreach my $offset (@offsets) {
    foreach my $format (@formats) {
      my $testline = "";

      my $num_failed = 0;

      foreach my $target (@targets) {
        my $tgtdata = $tests->{$test}->{$target};
        if ($tgtdata) {
          my $testres = $tgtdata->{$format}->{$offset};
          if ($testres) {
            my %testfiles = testfiles($test, $target, $format, $offset);
            $testline .= "<td class=\"$testres\">";
            $stats{$target}{$testres}++;
            if ($testres eq "PASS") {
              if ($config_show_all) {
                $testline .= "<a href=\"" . $testfiles{"out"} . "\"><img src=\"" . $testfiles{"out"} . "\"></a>";
              }
            } elsif ($testres eq "FAIL") {
              $num_failed++;

              if ($config_show_fail || $config_show_all) {
                $testline .= "<a href=\"" . $testfiles{"out"} . "\"><img src=\"" . $testfiles{"out"} . "\"></a>";
                $testline .= "<hr size=\"1\">";
                $testline .= "<a href=\"" . $testfiles{"diff"} . "\"><img src=\"" . $testfiles{"diff"} . "\"></a>";
              }
            } elsif ($testres eq "XFAIL") {
              #nothing
              if ($config_show_all) {
                $testline .= "<a href=\"" . $testfiles{"out"} . "\"><img src=\"" . $testfiles{"out"} . "\"></a>";
                $testline .= "<hr size=\"1\">";
                $testline .= "<a href=\"" . $testfiles{"diff"} . "\"><img src=\"" . $testfiles{"diff"} . "\"></a>";
              }
            }

            $testline .= "</td>";
          } else {
            $testline .= '<td></td>';
          }
        } else {
          $testline .= '<td></td>';
        }
      }

      my $testref = testref($test, $format);
      print '<tr><td>', "<a href=\"$testref\">", $test, ' (', $format, '/', $offset, ')</a></td>';

      if ($config_show_ref) {
        print "<td><a href=\"$testref\"><img src=\"$testref\"></img></a></td>";
      }

      print $testline;

      print "</tr>\n";
    }
  }
}

print "</table></body></html>\n";

