(0..127).each do |num|
  print "#{num}: "
  if num > 64
    puts 400 * (num - 64) / 63 + 64
  else
    puts num
  end
end
