class OpenStrip
  class Strip
    attr_accessor :dim, :program, :speed, :hold, :position, :fade, :num_colors, :opt_a, :opt_b, :colors

    PROGRAMS = {
      solid:    0,
      stripes:  2,
      wipe:     4,
      wipe_pp:  6,
      marquee:  8,
      strobe:  10,
      kitt:    12
    }

    def initialize
      @colors = (0..31).collect {Color.new}
    end

    def packet
      buff = [@dim, @program, @speed, @hold, @position, @fade, @num_colors, @opt_a, @opt_b].collect { |v| v || 0 }
      colors.each { |c| buff += c.values}
      buff.collect(&:chr).join
    end
  end

  class OpenStrip::Color
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
end
