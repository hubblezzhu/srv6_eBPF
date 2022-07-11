#include <stddef.h>
#include <inttypes.h>
#include <errno.h>
#include <linux/seg6_local.h>
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

/* Packet parsing state machine helpers. */
#define cursor_advance(_cursor, _len) \
	({ void *_tmp = _cursor; _cursor += _len; _tmp; })

#define SR6_FLAG_ALERT (1 << 4)

#define BPF_PACKET_HEADER __attribute__((packed))

struct ip6_t {
	unsigned int ver:4;
	unsigned int priority:8;
	unsigned int flow_label:20;
	unsigned short payload_len;
	unsigned char next_header;
	unsigned char hop_limit;
	unsigned long long src_hi;
	unsigned long long src_lo;
	unsigned long long dst_hi;
	unsigned long long dst_lo;
} BPF_PACKET_HEADER;

struct ip6_addr_t {
	unsigned long long hi;
	unsigned long long lo;
} BPF_PACKET_HEADER;

struct ip6_srh_t {
	unsigned char nexthdr;
	unsigned char hdrlen;
	unsigned char type;
	unsigned char segments_left;
	unsigned char first_segment;
	unsigned char flags;
	unsigned short tag;

	struct ip6_addr_t segments[0];
} BPF_PACKET_HEADER;

struct sr6_tlv_t {
	unsigned char type;
	unsigned char len;
	unsigned char value[0];
} BPF_PACKET_HEADER;

static __always_inline struct ip6_srh_t *get_srh(struct __sk_buff *skb)
{
	void *cursor, *data_end;
	struct ip6_srh_t *srh;
	struct ip6_t *ip;
	uint8_t *ipver;

	data_end = (void *)(long)skb->data_end;
	cursor = (void *)(long)skb->data;
	ipver = (uint8_t *)cursor;

	if ((void *)ipver + sizeof(*ipver) > data_end)
		return NULL;

	if ((*ipver >> 4) != 6)
		return NULL;

	ip = cursor_advance(cursor, sizeof(*ip));
	if ((void *)ip + sizeof(*ip) > data_end)
		return NULL;

	if (ip->next_header != 43)
		return NULL;

	srh = cursor_advance(cursor, sizeof(*srh));
	if ((void *)srh + sizeof(*srh) > data_end)
		return NULL;

	if (srh->type != 4)
		return NULL;

	return srh;
}

// Inspect if the Egress TLV and flag have been removed, if the tag is correct,
// then apply a End.T action to reach the last segment
SEC("dt6")
int __dt6(struct __sk_buff *skb)
{
	struct ip6_srh_t *srh = get_srh(skb);
	int table = 123;
	int err;

	if (srh == NULL)
		return BPF_DROP;

	// if (srh->flags != 0)
	// 	return BPF_DROP;

	// if (srh->tag != bpf_htons(2442))
	// 	return BPF_DROP;

	// if (srh->hdrlen != 8) // 4 segments
	// 	return BPF_DROP;

	err = bpf_lwt_seg6_action(skb, SEG6_LOCAL_ACTION_END_DT6,
				  (void *)&table, sizeof(table));

	if (err)
		return BPF_DROP;

	return BPF_REDIRECT;
}

char __license[] SEC("license") = "GPL";
