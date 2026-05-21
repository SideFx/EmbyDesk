/////////////////////////////////////////////////////////////////////////////
// Name:        Emby.go
// Purpose:     Emby types based on Emby SDK v4.9.3.0
// Author:      Jan Buchholz
// Created:     2026-04-13
// Changed:		2026-05-21
/////////////////////////////////////////////////////////////////////////////

package API

import "time"

// UserDto (simplified)
type UserDto struct {
	Name                  string `json:"Name"`
	Id                    string `json:"Id"`
	HasPassword           bool   `json:"HasPassword"`
	HasConfiguredPassword bool   `json:"HasConfiguredPassword"`
}

// AuthenticationResult (simplified)
type AuthenticationResult struct {
	User        UserDto `json:"UserDto"`
	AccessToken string  `json:"AccessToken"`
}

type BaseItemDto struct {
	Name                    string            `json:"Name"`
	OriginalTitle           string            `json:"OriginalTitle"`
	ServerId                string            `json:"ServerId"`
	Id                      string            `json:"Id"`
	Guid                    string            `json:"Guid"`
	Etag                    string            `json:"Etag"`
	Prefix                  string            `json:"Prefix"`
	PlaylistItemId          string            `json:"PlaylistItemId"`
	DateCreated             time.Time         `json:"DateCreated"`
	DateModified            time.Time         `json:"DateModified"`
	VideoCodec              string            `json:"VideoCodec"`
	AudioCodec              string            `json:"AudioCodec"`
	AverageFrameRate        float32           `json:"AverageFrameRate"`
	RealFrameRate           float32           `json:"RealFrameRate"`
	ExtraType               string            `json:"ExtraType"`
	SortIndexNumber         int32             `json:"SortIndexNumber"`
	SortParentIndexNumber   int32             `json:"SortParentIndexNumber"`
	Container               string            `json:"Container"`
	SortName                string            `json:"SortName"`
	ForcedSortName          string            `json:"ForcedSortName"`
	PremiereDate            time.Time         `json:"PremiereDate"`
	MediaSources            []MediaSourceInfo `json:"MediaSources"`
	CriticRating            float32           `json:"CriticRating"`
	ProductionLocations     []string          `json:"ProductionLocations"`
	Path                    string            `json:"Path"`
	OfficialRating          string            `json:"OfficialRating"`
	CustomRating            string            `json:"CustomRating"`
	Overview                string            `json:"Overview"`
	Taglines                []string          `json:"Taglines"`
	Genres                  []string          `json:"Genres"`
	CommunityRating         float32           `json:"CommunityRating"`
	RunTimeTicks            int64             `json:"RunTimeTicks"`
	Size                    int64             `json:"Size"`
	FileName                string            `json:"FileName"`
	Bitrate                 int32             `json:"Bitrate"`
	ProductionYear          int32             `json:"ProductionYear"`
	Number                  string            `json:"Number"`
	IndexNumber             int32             `json:"IndexNumber"`
	IndexNumberEnd          int32             `json:"IndexNumberEnd"`
	ParentIndexNumber       int32             `json:"ParentIndexNumber"`
	ProviderIds             map[string]string `json:"ProviderIds"`
	IsFolder                bool              `json:"IsFolder"`
	ParentId                string            `json:"ParentId"`
	Type                    string            `json:"Type"`
	People                  []BaseItemPerson  `json:"People"`
	Studios                 []NameLongIdPair  `json:"Studios"`
	GenreItems              []NameLongIdPair  `json:"GenreItems"`
	TagItems                []NameLongIdPair  `json:"TagItems"`
	ParentLogoItemId        string            `json:"ParentLogoItemId"`
	ParentBackdropItemId    string            `json:"ParentBackdropItemId"`
	ParentBackdropImageTags []string          `json:"ParentBackdropImageTags"`
	UserData                UserItemDataDto   `json:"UserData"`
	RecursiveItemCount      int32             `json:"RecursiveItemCount"`
	ChildCount              int32             `json:"ChildCount"`
	SeasonCount             int32             `json:"SeasonCount"`
	SeriesName              string            `json:"SeriesName"`
	SeriesId                string            `json:"SeriesId"`
	SeasonId                string            `json:"SeasonId"`
	SpecialFeatureCount     int32             `json:"SpecialFeatureCount"`
	DisplayPreferencesId    string            `json:"DisplayPreferencesId"`
	Status                  string            `json:"Status"`
	AirDays                 []string          `json:"AirDays"`
	Tags                    []string          `json:"Tags"`
	PrimaryImageAspectRatio float64           `json:"PrimaryImageAspectRatio"`
	Artists                 []string          `json:"Artists"`
	ArtistItems             []NameIdPair      `json:"ArtistItems"`
	Composers               []NameIdPair      `json:"Composers"`
	Album                   string            `json:"Album"`
	CollectionType          string            `json:"CollectionType"`
	DisplayOrder            string            `json:"DisplayOrder"`
	AlbumId                 string            `json:"AlbumId"`
	AlbumPrimaryImageTag    string            `json:"AlbumPrimaryImageTag"`
	SeriesPrimaryImageTag   string            `json:"AeriesPrimaryImageTag"`
	AlbumArtist             string            `json:"AlbumArtist"`
	AlbumArtists            []NameIdPair      `json:"AlbumArtists"`
	SeasonName              string            `json:"SeasonName"`
	MediaStreams            []MediaStream     `json:"MediaStreams"`
	PartCount               int32             `json:"PartCount"`
	ImageTags               map[string]string `json:"ImageTags"`
	BackdropImageTags       []string          `json:"BackdropImageTags"`
	ParentLogoImageTag      string            `json:"ParentLogoImageTag"`
	SeriesStudio            string            `json:"SeriesStudio"`
	PrimaryImageItemId      string            `json:"PrimaryImageItemId"`
	PrimaryImageTag         string            `json:"PrimaryImageTag"`
	ParentThumbItemId       string            `json:"ParentThumbItemId"`
	ParentThumbImageTag     string            `json:"ParentThumbImageTag"`
	Chapters                []ChapterInfo     `json:"Chapters"`
	LocationType            string            `json:"LocationType"`
	MediaType               string            `json:"MediaType"`
	EndDate                 time.Time         `json:"EndDate"`
	Width                   int32             `json:"Width"`
	Height                  int32             `json:"Height"`
	CameraMake              string            `json:"CameraMake"`
	CameraModel             string            `json:"CameraModel"`
	Software                string            `json:"Software"`
	ExposureTime            float64           `json:"ExposureTime"`
	FocalLength             float64           `json:"FocalLength"`
	ImageOrientation        string            `json:"ImageOrientation"`
	Aperture                float64           `json:"Aperture"`
	ShutterSpeed            float64           `json:"ShutterSpeed"`
	Latitude                float64           `json:"Latitude"`
	Longitude               float64           `json:"Longitude"`
	Altitude                float64           `json:"Altitude"`
	IsoSpeedRating          int32             `json:"IsoSpeedRating"`
	SeriesTimerId           string            `json:"SeriesTimerId"`
	EpisodeTitle            string            `json:"EpisodeTitle"`
	IsMovie                 bool              `json:"IsMovie"`
	IsSports                bool              `json:"IsSports"`
	IsSeries                bool              `json:"IsSeries"`
	IsNews                  bool              `json:"IsNews"`
	IsKids                  bool              `json:"IsKids"`
	IsPremiere              bool              `json:"IsPremiere"`
	Disabled                bool              `json:"Disabled"`
	ManagementId            string            `json:"ManagementId"`
	MovieCount              int32             `json:"MovieCount"`
	SeriesCount             int32             `json:"SeriesCount"`
	AlbumCount              int32             `json:"AlbumCount"`
	SongCount               int32             `json:"SongCount"`
	MusicVideoCount         int32             `json:"MusicVideoCount"`
	Subviews                []string          `json:"Subviews"`
}

type BaseItemPerson struct {
	Name            string `json:"Name"`
	Id              string `json:"Id"`
	Role            string `json:"Role"`
	Type            string `json:"Type"`
	PrimaryImageTag string `json:"PrimaryImageTag"`
}

type ChapterInfo struct {
	StartPositionTicks int64  `json:"StartPositionTicks"`
	Name               string `json:"Name"`
	ImageTag           string `json:"ImageTag"`
	MarkerType         string `json:"MarkerType"`
	ChapterIndex       int32  `json:"ChapterIndex"`
}

type MediaSourceInfo struct {
	Chapters                   []ChapterInfo     `json:"Chapters"`
	Protocol                   string            `json:"Protocol"`
	Id                         string            `json:"Id"`
	Path                       string            `json:"Path"`
	EncoderPath                string            `json:"EncoderPath"`
	EncoderProtocol            string            `json:"EncoderProtocol"`
	Type                       string            `json:"Type"`
	ProbePath                  string            `json:"ProbePath"`
	ProbeProtocol              string            `json:"ProbeProtocol"`
	Container                  string            `json:"Container"`
	Size                       int64             `json:"Size"`
	Name                       string            `json:"Name"`
	SortName                   string            `json:"SortName"`
	IsRemote                   bool              `json:"IsRemote"`
	HasMixedProtocols          bool              `json:"HasMixedProtocols"`
	RunTimeTicks               int64             `json:"RunTimeTicks"`
	ContainerStartTimeTicks    int64             `json:"ContainerStartTimeTicks"`
	SupportsTranscoding        bool              `json:"SupportsTranscoding"`
	TrancodeLiveStartIndex     int32             `json:"TrancodeLiveStartIndex"`
	WallClockStart             time.Time         `json:"WallClockStart"`
	SupportsDirectStream       bool              `json:"SupportsDirectStream"`
	SupportsDirectPlay         bool              `json:"SupportsDirectPlay"`
	IsInfiniteStream           bool              `json:"IsInfiniteStream"`
	RequiresOpening            bool              `json:"RequiresOpening"`
	OpenToken                  string            `json:"OpenToken"`
	RequiresClosing            bool              `json:"RequiresClosing"`
	LiveStreamId               string            `json:"LiveStreamId"`
	BufferMs                   int32             `json:"BufferMs"`
	RequiresLooping            bool              `json:"RequiresLooping"`
	SupportsProbing            bool              `json:"SupportsProbing"`
	MediaStreams               []MediaStream     `json:"MediaStreams"`
	Formats                    []string          `json:"Formats"`
	Bitrate                    int32             `json:"Bitrate"`
	Timestamp                  string            `json:"Timestamp"`
	RequiredHttpHeaders        map[string]string `json:"RequiredHttpHeaders"`
	DirectStreamUrl            string            `json:"DirectStreamUrl"`
	AddApiKeyToDirectStreamUrl bool              `json:"AddApiKeyToDirectStreamUrl"`
	TranscodingUrl             string            `json:"TranscodingUrl"`
	TranscodingSubProtocol     string            `json:"TranscodingSubProtocol"`
	TranscodingContainer       string            `json:"TranscodingContainer"`
	AnalyzeDurationMs          int32             `json:"AnalyzeDurationMs"`
	ReadAtNativeFramerate      bool              `json:"ReadAtNativeFramerate"`
	DefaultAudioStreamIndex    int32             `json:"DefaultAudioStreamIndex"`
	DefaultSubtitleStreamIndex int32             `json:"DefaultSubtitleStreamIndex"`
	ItemId                     string            `json:"ItemId"`
	ServerId                   string            `json:"ServerId"`
}

type MediaStream struct {
	Codec                           string  `json:"Codec"`
	CodecTag                        string  `json:"CodecTag"`
	Language                        string  `json:"Language"`
	ColorTransfer                   string  `json:"ColorTransfer"`
	ColorPrimaries                  string  `json:"ColorPrimaries"`
	ColorSpace                      string  `json:"ColorSpace"`
	Comment                         string  `json:"Comment"`
	StreamStartTimeTicks            int64   `json:"StreamStartTimeTicks"`
	TimeBase                        string  `json:"TimeBase"`
	Title                           string  `json:"Title"`
	Extradata                       string  `json:"Extradata"`
	VideoRange                      string  `json:"VideoRange"`
	DisplayTitle                    string  `json:"DisplayTitle"`
	DisplayLanguage                 string  `json:"DisplayLanguage"`
	IsInterlaced                    bool    `json:"IsInterlaced"`
	IsAVC                           bool    `json:"IsAVC"`
	ChannelLayout                   string  `json:"ChannelLayout"`
	BitRate                         int32   `json:"BitRate"`
	BitDepth                        int32   `json:"BitDepth"`
	RefFrames                       int32   `json:"RefFrames"`
	Rotation                        int32   `json:"Rotation"`
	Channels                        int32   `json:"Channels"`
	SampleRate                      int32   `json:"SampleRate"`
	IsDefault                       bool    `json:"IsDefault"`
	IsForced                        bool    `json:"IsForced"`
	IsHearingImpaired               bool    `json:"IsHearingImpaired"`
	Height                          int32   `json:"Height"`
	Width                           int32   `json:"Width"`
	AverageFrameRate                float32 `json:"AverageFrameRate"`
	RealFrameRate                   float32 `json:"RealFrameRate"`
	Profile                         string  `json:"Profile"`
	Type                            string  `json:"Type"`
	AspectRatio                     string  `json:"AspectRatio"`
	Index                           int32   `json:"Index"`
	IsExternal                      bool    `json:"IsExternal"`
	DeliveryMethod                  string  `json:"DeliveryMethod"`
	DeliveryUrl                     string  `json:"DeliveryUrl"`
	IsExternalUrl                   bool    `json:"IsExternalUrl"`
	IsChunkedResponse               bool    `json:"IsChunkedResponse"`
	IsTextSubtitleStream            bool    `json:"IsTextSubtitleStream"`
	SupportsExternalStream          bool    `json:"SupportsExternalStream"`
	Path                            string  `json:"Path"`
	Protocol                        string  `json:"Protocol"`
	PixelFormat                     string  `json:"PixelFormat"`
	Level                           float64 `json:"Level"`
	IsAnamorphic                    bool    `json:"IsAnamorphic"`
	ExtendedVideoType               string  `json:"ExtendedVideoType"`
	ExtendedVideoSubType            string  `json:"ExtendedVideoSubType"`
	ExtendedVideoSubTypeDescription string  `json:"ExtendedVideoSubTypeDescription"`
	ItemId                          string  `json:"ItemId"`
	ServerId                        string  `json:"ServerId"`
	AttachmentSize                  int32   `json:"AttachmentSize"`
	MimeType                        string  `json:"MimeType"`
	SubtitleLocationType            string  `json:"SubtitleLocationType"`
}

type NameIdPair struct {
	Name string `json:"Name"`
	Id   string `json:"Id"`
}

type NameLongIdPair struct {
	Name string `json:"Name"`
	Id   int64  `json:"Id"`
}

type QueryResultBaseItemDto struct {
	Items            []BaseItemDto `json:"Items"`
	TotalRecordCount int32         `json:"TotalRecordCount"`
}

type UserItemDataDto struct {
	Rating                float64   `json:"Rating"`
	PlayedPercentage      float64   `json:"PlayedPercentage"`
	UnplayedItemCount     int32     `json:"UnplayedItemCount"`
	PlaybackPositionTicks int64     `json:"PlaybackPositionTicks"`
	PlayCount             int32     `json:"PlayCount"`
	IsFavorite            bool      `json:"IsFavorite"`
	LastPlayedDate        time.Time `json:"LastPlayedDate"`
	Played                bool      `json:"Played"`
	Key                   string    `json:"Key"`
	ItemId                string    `json:"ItemId"`
	ServerId              string    `json:"ServerId"`
}

const (
	AudioMediaStreamType string = "Audio"
	VideoMediaStreamType string = "Video"
)

const (
	ActorPersonType     string = "Actor"
	GuestStarPersonType string = "GuestStar"
	DirectorPersonType  string = "Director"
	AnyPersonType       string = "AnyPerson" //does not belong to Emby API
)
