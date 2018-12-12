require 'mqtt'

class OpenStrip
  class Color
    attr_accessor :h, :s, :v
    def values
      [@h || 0, @s || 0, @v || 0]
    end

    def set(x, y, z)
      @h = x
      @s = y
      @v = z
    end
  end

  attr_accessor :dim, :program, :speed, :hold, :position, :fade, :num_colors, :opt_a, :opt_b, :colors

  def initialize
    @colors = (0..31).collect { Color.new }
  end

  def packet
    buff = [@dim, @program, @speed, @hold, @position, @fade, @num_colors, @opt_a, @opt_b].collect { |v| v || 0 }
    colors.each { |c| buff += c.values}
    buff.collect(&:chr).join
  end
end

o = OpenStrip.new
o.dim = 255
o.program = 2
o.speed = 80
o.position = 127
o.num_colors = 5
o.opt_a = 218
o.colors[0].set(50,255,255)
o.colors[1].set(163,255,255)
o.colors[2].set(94,255,255)
o.colors[3].set(0,255,255)
o.colors[4].set(189,255,255)

oo = OpenStrip.new
oo.dim = 255
oo.program = 0
oo.speed = 120
oo.position = 127
oo.num_colors = 5
oo.fade = 29
oo.colors[0].set(0,255,255)
oo.colors[1].set(163,255,255)
oo.colors[2].set(94,255,255)
oo.colors[3].set(50,255,255)
oo.colors[4].set(189,255,255)

MQTT::Client.connect('192.168.15.10') do |c|
  packet = oo.packet + o.packet
  c.publish('openStrip/dev/OpenStrip-80C40A24', packet, false, 1)
  #c.publish('/openStrip/demo', o.packet)
end
